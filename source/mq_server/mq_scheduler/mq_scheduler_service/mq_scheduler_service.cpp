#include "sdp/pch.h"
#include <ranges>

#include "sdp/configure/configure.h"

#include "mq_scheduler_service/mq_scheduler_service.h"

CMqSchedulerService::CForwarderAddrSession::CForwarderAddrSession(const uint16_t nNodeType, const uint16_t nNodeID, const int32_t nForwarderNum, const uint16_t nPort, const SDP::CSdpString& strAddr)
	: m_nNodeType(nNodeType)
	, m_nNodeID(nNodeID)
	, m_nForwarderNum(nForwarderNum)
	, m_nPort(nPort)
	, m_strAddr(strAddr)
{
}

bool CMqSchedulerService::CForwarderAddrSession::operator<(const CForwarderAddrSession& rForwarderAddrSession) const
{
	return std::tie(m_nPort, m_strAddr) < std::tie(rForwarderAddrSession.m_nPort, rForwarderAddrSession.m_strAddr);
}

CMqSchedulerService::CMqSchedulerServiceHandler::CMqSchedulerServiceHandler(CMqSchedulerService& rService, const SOS::handle_type handle, const uint32_t nHandleID, const std::uint32_t nMsgBufSize)
	: CMcpServerHandler(rService, handle, nHandleID, nMsgBufSize)
	, m_nClientID(0)
{
}

void CMqSchedulerService::CMqSchedulerServiceHandler::HandleException()
{
	this->GetService().OnServiceNetworkError(this);
}

int32_t CMqSchedulerService::CMqSchedulerServiceHandler::OnGetMessage(char* pBuffer, const SOS::s_size_t nBufferLen, int32_t& nBytes)
{
	return this->GetService().OnHtsMessage(pBuffer, nBufferLen, nBytes, this);
}

CMqSchedulerService::CMqSchedulerService()
	: m_mqSchedulerServiceAcceptor(*this)
{
}

int32_t CMqSchedulerService::OnInit()
{
	if (SDP::CHtsServerService::OnInit())
	{
		SDP_RUN_LOG_ERROR("SDP::CHtsServerService::OnInit() failed!");

		return -1;
	}

	SDP::CSdpString strIpAddr;
	int32_t nPort = 0;

	if (-1 == SDP::CFG::GetProfileStr("mq", "mq_scheduler_service_ip_addr", strIpAddr))
	{
		SDP_RUN_LOG_WARNING("Unable to read configuration value for 'mq_scheduler_service_ip_addr'.");

		return -1;
	}

	if (-1 == SDP::CFG::GetProfileInteger("mq", "mq_scheduler_service_port", nPort))
	{
		SDP_RUN_LOG_WARNING("Unable to read configuration value for 'mq_scheduler_service_port'.");

		return -1;
	}

	if (m_mqSchedulerServiceAcceptor.Open(strIpAddr, static_cast<uint16_t>(nPort), SDP::PROT_TYPE::EN_INTERNET_TCP_TYPE))
	{
		SDP_RUN_LOG_WARNING("Failed to open connection, IP address: {}, Port: {}", strIpAddr.c_str(), nPort);
		return -1;
	}

	SDP_RUN_LOG_INFO("CMqSchedulerService initialized successfully! IP address: {}, Port: {}", strIpAddr.c_str(), nPort);
	return 0;
}

int32_t CMqSchedulerService::OnExit()
{
	m_mqSchedulerServiceAcceptor.Close();

	for (const auto val : m_mapMqClientHandlerID | std::views::values)
	{
		this->ReleaseHandler(val);
	}

	return SDP::CHtsServerService::OnExit();
}

void CMqSchedulerService::OnSdpMsg(const MQ_MSG::CRegisterMqForwarderServiceReq& req, const SDP::CSdpMsgAddr& rSrcMsgAddr)
{
	SDP_MESSAGE_TRACE("Received MQ_MSG::CRegisterMqForwarderServiceReq");

	const CForwarderAddrSession forwarderAddr(rSrcMsgAddr.GetNodeType(), rSrcMsgAddr.GetNodeID(), req.m_nForwarderNum, req.m_nPort, req.m_strAddr);

	if (const auto res = m_setForwarderInfo.insert(forwarderAddr); res.second)
	{
		MQ_MSG::CAddForwarderAddrNotify notify;
		auto& [nPort, nForwarderNum, strIpAddr] = notify.m_mqForwarderInfo;

		nPort = forwarderAddr.m_nPort;
		nForwarderNum = forwarderAddr.m_nForwarderNum;
		strIpAddr = forwarderAddr.m_strAddr;

		for (const auto pHandler : m_mapMqClientHandlerID | std::views::values)
		{
			this->SendHtsMsg(notify, pHandler);
		}
	}
	else
	{
		SDP_RUN_LOG_WARNING("Duplicate entry detected for forwarder. IP address: {}, Port: {}", req.m_strAddr.c_str(), req.m_nPort);
	}
}

int32_t CMqSchedulerService::OnHtsMsg(MQ_MSG::CRegMqClientAddrReq& rRegMqClientAddrReq, CMqSchedulerServiceHandler* pHandler)
{
	SDP_MESSAGE_TRACE("Received MQ_MSG::CRegMqClientAddrReq");

	if (const auto it = m_mapMqClientHandlerID.find(rRegMqClientAddrReq.m_nClientID); it != m_mapMqClientHandlerID.end())
	{
		CMqSchedulerServiceHandler* pOldHandler = it->second;

		SDP_RUN_LOG_WARNING("Duplicate client detected. Kicking off old client with ID: {}", pOldHandler->m_nClientID);
		this->ReleaseHandler(pOldHandler);
		m_mapMqClientHandlerID.erase(it);
	}

	pHandler->m_nClientID = rRegMqClientAddrReq.m_nClientID;
	m_mapMqClientHandlerID.emplace(rRegMqClientAddrReq.m_nClientID, pHandler);

#if CPP_STANDARD >= 20
	MQ_MSG::CRegMqClientAddrRsp rsp = {.m_nClientID = rRegMqClientAddrReq.m_nClientID, .m_lstMqForwarderInfo = {}};
#else
	MQ_MSG::CRegMqClientAddrRsp rsp = {rRegMqClientAddrReq.m_nClientID, {}};
#endif

	for (const auto& session : m_setForwarderInfo)
	{
#if CPP_STANDARD >= 20
		MQ_MSG::CMqForwarderInfo addr = {.m_nPort = session.m_nPort, .m_nForwarderNum = session.m_nForwarderNum, .m_strIpAddr = session.m_strAddr};
#else
		MQ_MSG::CMqForwarderInfo addr = {session.m_nPort, session.m_nForwarderNum, session.m_strAddr};
#endif

		rsp.m_lstMqForwarderInfo.push_front(addr);
	}

	this->SendHtsMsg(rsp, pHandler);

	return 0;
}

void CMqSchedulerService::OnServiceNetworkError(CMqSchedulerServiceHandler* pHandler)
{
	m_mapMqClientHandlerID.erase(pHandler->m_nClientID);

	this->ReleaseHandler(pHandler);
}

void CMqSchedulerService::OnDisConnected(const SDP::CConnectInfo& rNetInfo)
{
	for (auto it = m_setForwarderInfo.begin(); it != m_setForwarderInfo.end();)
	{
		if (it->m_nNodeType == rNetInfo.m_remoteConnectInfo.m_nType && it->m_nNodeID == rNetInfo.m_remoteConnectInfo.m_nID)
		{
			MQ_MSG::CDelForwarderAddrNotify notify;
			auto& [nPort, nForwarderNum, strIpAddr] = notify.m_mqForwarderInfo;

			nPort = it->m_nPort;
			nForwarderNum = it->m_nForwarderNum;
			strIpAddr = it->m_strAddr;

			for (const auto val : m_mapMqClientHandlerID | std::views::values)
			{
				CMqSchedulerServiceHandler* pHandler = val;

				this->SendHtsMsg(notify, pHandler);
			}

			it = m_setForwarderInfo.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void CMqSchedulerService::OnAccept(const SOS::handle_type handle)
{
	this->CreateAndRegisterHandler<CMqSchedulerService, CMqSchedulerServiceHandler>(*this, handle, CMqSchedulerServiceHandler::k_DEFAULT_MSG_BUF_SIZE);
}
#include "sdp/pch.h"
#include "sdp/configure/configure.h"

#include "mq_forwarder_service/mq_forwarder_scheduler.h"

CMqForwarderScheduler::CMqForwarderSchedulerHandler::CMqForwarderSchedulerHandler(CMqForwarderScheduler& rScheduler, const SOS::handle_type handle, const std::uint32_t nHandleID, const std::uint32_t nMsgBufSize)
    : CMcpServerHandler(rScheduler, handle, nHandleID, nMsgBufSize)
{
}

int32_t CMqForwarderScheduler::CMqForwarderSchedulerHandler::OnGetMessage(char* pBuffer, const SOS::s_size_t nBufferLen, int32_t& nBytes)
{
    return this->GetService().OnHtsMessage(pBuffer, nBufferLen, nBytes, this);
}

void CMqForwarderScheduler::CMqForwarderSchedulerHandler::HandleException()
{
    this->GetService().OnServiceNetworkError(this);
}

CMqForwarderScheduler::CMqForwarderScheduler(const std::int32_t nMqForwardNum)
    : m_nMqForwarderPort(0)
    , m_nMqForwardNum(nMqForwardNum)
    , m_mqAcceptor(*this)
{
}

std::int32_t CMqForwarderScheduler::OnInit()
{
    if (SDP::CHtsServerService::OnInit())
    {
        return -1;
    }

    if (-1 == SDP::CFG::GetProfileStr("mq", "mq_forwarder_ip_addr", m_strMqForwarderAddr))
    {
		SDP_RUN_LOG_WARNING("Unable to read 'mq_forwarder_ip_addr' configuration item value.");

        return -1;
    }

    if (-1 == SDP::CFG::GetProfileInteger("mq", "mq_forwarder_port", m_nMqForwarderPort))
    {
		SDP_RUN_LOG_WARNING("Unable to read 'mq_forwarder_port' configuration item value.");

        return -1;
    }

    if (m_mqAcceptor.Open(m_strMqForwarderAddr, static_cast<uint16_t>(m_nMqForwarderPort), SDP::PROT_TYPE::EN_INTERNET_TCP_TYPE))
    {
		SDP_RUN_LOG_WARNING("Failed to listen on IP address: {}, port: {}", m_strMqForwarderAddr.c_str(), m_nMqForwarderPort);

        return -1;
    }

    return 0;
}

std::int32_t CMqForwarderScheduler::OnExit()
{
    m_mqAcceptor.Close();

    return SDP::CHtsServerService::OnExit();
}

std::int32_t CMqForwarderScheduler::OnHtsMsg(const MQ_MSG::CRegMqTopicReq& rRegMqTopicReq, CMqForwarderSchedulerHandler* pHandler)
{
    std::uint16_t fwd_id = 0;

    if (const auto it = m_mapCltID2FwdID.find(rRegMqTopicReq.m_nClientID); it == m_mapCltID2FwdID.end())
    {
        fwd_id = 1;
        m_mapCltID2FwdID.emplace(rRegMqTopicReq.m_nClientID, fwd_id);
    }
    else
    {
        std::uint16_t& id = it->second;

        ++id;
        if (id > m_nMqForwardNum)
        {
            id = 1;
        }
        fwd_id = id;
    }

    const SDP::CSdpMsgAddr addr(MQ_ID::EN_MQ_FORWARDER_SERVICE_TYPE, fwd_id);
#if CPP_STANDARD >= 20
    MQ_MSG::CForwardRegMqTopicReq req = {.m_handle = pHandler->GetHandle(), .m_regMqTopicReq = rRegMqTopicReq};
#else
	MQ_MSG::CForwardRegMqTopicReq req = {pHandler->GetHandle(), rRegMqTopicReq};
#endif

    if (this->SendMsg(req, addr))
    {
        this->OnServiceNetworkError(pHandler);

        return -1;
    }

    pHandler->UnregisterHandler(SOS::EN_ALL_EVENTS_MASK);
    pHandler->SetHandle(SDP_INVALID_HANDLE);
    SDP_DELETE(pHandler);

    return 1;
}

void CMqForwarderScheduler::OnConnected(const SDP::CConnectInfo& rNetInfo)
{
    if (rNetInfo.m_remoteConnectInfo.m_nType == MQ_ID::EN_MQ_SCHEDULER_NODE_TYPE)
    {
#if CPP_STANDARD >= 20
        MQ_MSG::CRegisterMqForwarderServiceReq req = {.m_nPort = static_cast<uint16_t>(m_nMqForwarderPort), .m_nForwarderNum = m_nMqForwardNum, .m_strAddr = m_strMqForwarderAddr};
#else
		MQ_MSG::CRegisterMqForwarderServiceReq req = {static_cast<uint16_t>(m_nMqForwarderPort), m_nMqForwardNum, m_strMqForwarderAddr};
#endif
        const SDP::CSdpMsgAddr addr(MQ_ID::EN_MQ_SCHEDULER_NODE_TYPE, MQ_ID::EN_MQ_SCHEDULER_NODE_ID, MQ_ID::EN_MQ_SCHEDULER_SERVICE_TYPE, MQ_ID::EN_MQ_SCHEDULER_SERVICE_ID);

        this->SendMsg(req, addr);
    }
}

void CMqForwarderScheduler::OnServiceNetworkError(CMqForwarderSchedulerHandler* pHandler) const
{
    this->ReleaseHandler(pHandler);
}

void CMqForwarderScheduler::OnAccept(const SOS::handle_type handle)
{
    this->CreateAndRegisterHandler<CMqForwarderScheduler, CMqForwarderSchedulerHandler>(*this, handle, CMqForwarderSchedulerHandler::k_DEFAULT_MSG_BUF_SIZE);
}
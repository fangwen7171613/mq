#include "sdp/pch.h"
#include "sdp/configure/configure.h"
#include "sdp/dispatcher/dispatcher_center.h"
#include "sdp/configure/yaml_bind_macros.h"

#include "mq_c_sdk/mcs_forwarder_agent.h"
#include "mq_c_sdk/mcs_forwarder_scheduler.h"

CMcsForwarderScheduler::CMqSchedulerHandler::CMqSchedulerHandler(CMcsForwarderScheduler& rService, const SOS::handle_type handle, const uint32_t nHandleID, const SDP::CIpAddr& rRemoteAddr,
                                                                 const SDP::CIpAddr& localAddr, const SDP::PROT_TYPE nProtType, const std::uint32_t nMsgBufSize)
    : CMcpClientHandler(rService, handle, nHandleID, rRemoteAddr, localAddr, nProtType, nMsgBufSize)
{
}

CMcsForwarderScheduler::CMcsForwarderScheduler(const MqMsgCb* pMqMsgCallback, const std::uint32_t nClientID, const SDP::list_t<std::uint32_t>& lstTopicID)
    : m_nAllocID(1)
    , m_nMqForwardNum(0)
    , m_nMcsReadyNum(0)
    , m_pMqMsgCallback(pMqMsgCallback)
    , m_pHandler(nullptr)
    , m_regMqTopicReq{nClientID, lstTopicID}
{
}

CMcsForwarderScheduler::~CMcsForwarderScheduler() = default;

namespace
{
    class CMqSchedulerAddr
    {
    public:
        YAML_BIND_ATTR(mq_scheduler_service_ip_addr, mq_scheduler_service_port)

        SDP::CSdpString mq_scheduler_service_ip_addr;
        std::int32_t mq_scheduler_service_port;
    };
} // namespace

std::int32_t CMcsForwarderScheduler::OnInit()
{
    if (SDP::CHtsClientService::OnInit())
    {
		SDP_RUN_LOG_WARNING("Failed to initialize SDP::CHtsClientService in OnInit()!");

        return -1;
    }

    CMqSchedulerAddr addr;
    if (-1 == SDP::CFG::GetProfileNode("mq", addr))
    {
		SDP_RUN_LOG_WARNING("Failed to read mq scheduler from configuration file.");

        return -1;
    }

    SDP::CIpAddr schedulerAddr(addr.mq_scheduler_service_ip_addr, static_cast<uint16_t>(addr.mq_scheduler_service_port));

    this->DoConnect(schedulerAddr, SDP::PROT_TYPE::EN_INTERNET_TCP_TYPE);

    return 0;
}

std::int32_t CMcsForwarderScheduler::OnExit()
{
    if (m_pHandler)
    {
        this->ReleaseHandler(m_pHandler);
    }

    return SDP::CHtsClientService::OnExit();
}

void CMcsForwarderScheduler::OnSdpMsg([[maybe_unused]] const MQ_MSG::CAgentReadyMsg& rMsg)
{
    ++m_nMcsReadyNum;

    if (m_nMcsReadyNum >= m_nMqForwardNum)
    {
        m_pMqMsgCallback->m_pOnMqCommEst();
    }
}

void CMcsForwarderScheduler::OnSdpMsg([[maybe_unused]] const MQ_MSG::CAgentNotReadyMsg& rMsg)
{
    SDP_ASSERT(m_nMcsReadyNum > 0);

    if (m_nMcsReadyNum > 0)
    {
        --m_nMcsReadyNum;
    }

    if (0 == m_nMcsReadyNum)
    {
        m_pMqMsgCallback->m_pOnMqCommIntr();
    }
}

std::int32_t CMcsForwarderScheduler::OnHtsMsg(const MQ_MSG::CRegMqClientAddrRsp& rRegMqClientAddrRsp)
{
    SDP_MESSAGE_TRACE("recv MQ_MSG::CRegMqClientAddrRsp");

    for (auto& info : rRegMqClientAddrRsp.m_lstMqForwarderInfo)
    {
        AddMqForwarderAddr(info);
    }

    return 0;
}

std::int32_t CMcsForwarderScheduler::OnHtsMsg(const MQ_MSG::CAddForwarderAddrNotify& rAddForwarderAddrNotify)
{
    AddMqForwarderAddr(rAddForwarderAddrNotify.m_mqForwarderInfo);

    return 0;
}

std::int32_t CMcsForwarderScheduler::OnHtsMsg(const MQ_MSG::CDelForwarderAddrNotify& rDelForwarderAddrNotify)
{
    SDP_MESSAGE_TRACE("Received MQ_MSG::CDelForwarderAddrNotify message.");

    const MQ_MSG::CMqForwarderInfo& fwInfo = rDelForwarderAddrNotify.m_mqForwarderInfo;
    const SDP::CIpAddr ipAddr(fwInfo.m_strIpAddr, fwInfo.m_nPort);

    if (const auto it = m_mapFwdInfo.find(ipAddr); it == m_mapFwdInfo.end())
    {
        SDP_RUN_LOG_WARNING("Forwarder address not found in map. IP: {}, Port: {}", fwInfo.m_strIpAddr.c_str(), fwInfo.m_nPort);
    }
    else
    {
        const auto& [nForwarderNum, nAgtID] = it->second;
        std::uint16_t nTempAgtID = nAgtID;

        for (int i = 0; i < nForwarderNum; ++i)
        {
            SDP::CSdpMsgAddr addr(CMcsForwarderAgent::EN_SERVICE_TYPE, nTempAgtID);
            MQ_MSG::CMcsExitAgentNotify notify;

            notify.m_nPort = fwInfo.m_nPort;
            notify.m_strIpAddr = fwInfo.m_strIpAddr;

            SendMsg(notify, addr);

            if (ITask* pMcA = SDP::CDispatcherCenterDemon::Instance()->UnregisterTask(CMcsForwarderAgent::EN_SERVICE_TYPE, nTempAgtID, false))
            {
                pMcA->Exit(SDP::EN_EXIT_TASK);
                --m_nMqForwardNum;
            }
            else
            {
                SDP_RUN_LOG_ERROR("Failed to unregister CMcsAgentService. Service Type: {}, Agent ID: {}",
                                  CMcsForwarderAgent::EN_SERVICE_TYPE, nTempAgtID);
            }
            ++nTempAgtID;
        }

        m_mapFwdInfo.erase(it);
    }

    return 0;
}

void CMcsForwarderScheduler::AddMqForwarderAddr(const MQ_MSG::CMqForwarderInfo& fwInfo)
{
    const SDP::CIpAddr ipAddr(fwInfo.m_strIpAddr, fwInfo.m_nPort);
#if CPP_STANDARD >= 20
    CFwdInfo fwdInfo = {.m_nForwarderNum = fwInfo.m_nForwarderNum, .m_nAgtID = m_nAllocID};
#else
	CFwdInfo fwdInfo = {fwInfo.m_nForwarderNum, m_nAllocID};
#endif
    const auto it = m_mapFwdInfo.emplace(ipAddr, fwdInfo);

    if (it.second)
    {
        for (std::int32_t i = 0; i < fwInfo.m_nForwarderNum; ++i)
        {
            if (const auto pAgent = new(std::nothrow) CMcsForwarderAgent(m_pMqMsgCallback, m_regMqTopicReq.m_nClientID, m_regMqTopicReq.m_lstTopicID, ipAddr))
            {
                if (SDP::CDispatcherCenterDemon::Instance()->RegisterTask(pAgent, CMcsForwarderAgent::EN_SERVICE_TYPE, m_nAllocID++, true))
                {
                    SDP_RUN_LOG_ERROR(
                        "Failed to register CMcsForwarderAgent service. Service Type: {}, Agent ID: {}",
                        CMcsForwarderAgent::EN_SERVICE_TYPE,
                        pAgent->GetTaskID());
                }
                else
                {
                    ++m_nMqForwardNum;
                }
            }
            else
            {
                SDP_RUN_LOG_ERROR("Memory allocation for CMcsForwarderAgent failed.");
            }
        }
    }
}

void CMcsForwarderScheduler::OnServiceNetworkError(CMqSchedulerHandler* pHandler)
{
    SDP_ASSERT(m_pHandler == pHandler);
	SDP::CHtsClientService::OnClientNetworkError(pHandler);
    m_pHandler = nullptr;
}

bool CMcsForwarderScheduler::OnMcpConnected(const SOS::handle_type handle, const std::uint32_t nHandleID, const SDP::CIpAddr& rRemoteAddr, const SDP::CIpAddr& localAddr, const SDP::PROT_TYPE nProtType)
{
    SDP_ASSERT(m_pHandler == nullptr);
    if (const auto pHandler = SDP_NEW(CMqSchedulerHandler, *this, handle, nHandleID, rRemoteAddr, localAddr, nProtType, CMqSchedulerHandler::k_DEFAULT_MSG_BUF_SIZE))
    {
        if (pHandler->RegisterHandler(SOS::EN_READ_MASK))
        {
			m_pHandler = pHandler;
			this->OnServiceNetworkError(pHandler);

            return false;
        }

        SDP_MESSAGE_TRACE("send MQ_MSG::CRegMqClientAddrReq");

        m_pHandler = pHandler;
		if (MQ_MSG::CRegMqClientAddrReq regMqClientAddrReq = {m_regMqTopicReq.m_nClientID}; SDP::SendStatus::EN_FAILED == this->SendHtsMsg(regMqClientAddrReq, pHandler))
        {
			this->OnServiceNetworkError(pHandler);
			SDP_RUN_LOG_WARNING("send CRegMqClientAddrReq failed");
        }
    }

    return true;
}
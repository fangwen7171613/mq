#include "sdp/pch.h"
#include "sdp/configure/configure.h"

#include "mq_c_sdk/mcs_forwarder_agent.h"

CMcsForwarderAgent::CMqForwarderAgentHandler::CMqForwarderAgentHandler(CMcsForwarderAgent& rService, const SOS::handle_type handle, const uint32_t nHandleID, const SDP::CIpAddr& rRemoteAddr,
                                                                       const SDP::CIpAddr& localAddr, const SDP::PROT_TYPE nProtType, const std::uint32_t nMsgBufSize)
    : CMcpClientHandler(rService, handle, nHandleID, rRemoteAddr, localAddr, nProtType, nMsgBufSize)
{
}

void CMcsForwarderAgent::CMqForwarderAgentHandler::HandleException()
{
    this->GetService().OnServiceNetworkError(this);
}

CMcsForwarderAgent::CMcsForwarderAgent(const MqMsgCb* pMqMsgCallback, const uint32_t nClientID, const SDP::list_t<uint32_t>& lstTopicID, const SDP::CIpAddr& forwarderIpAddr)
    : m_nTcpNodelayFlag(0)
    , m_pMqMsgCallback(pMqMsgCallback)
    , m_pHandler(nullptr)
    , m_regMqTopicReq{nClientID, lstTopicID}
    , m_forwarderIpAddr(forwarderIpAddr)
{
}

CMcsForwarderAgent::~CMcsForwarderAgent() = default;

int32_t CMcsForwarderAgent::OnInit()
{
    if (SDP::CHtsClientService::OnInit())
    {
        SDP_RUN_LOG_ERROR("SDP::CHtsClientService::OnInit() initialization failed!");

        return -1;
    }

    this->SetAffinity();

    if (-1 == SDP::CFG::GetProfileInteger("mq", "tcp_nodelay_flag", m_nTcpNodelayFlag))
    {
        SDP_RUN_LOG_WARNING("Unable to read 'tcp_nodelay_flag' configuration item. TCP no delay flag will not be set.");
        m_nTcpNodelayFlag = 0;
    }

    this->DoConnect(m_forwarderIpAddr, SDP::PROT_TYPE::EN_INTERNET_TCP_TYPE);

    return 0;
}

int32_t CMcsForwarderAgent::OnExit()
{
    this->CancelConnect(m_forwarderIpAddr);

    if (m_pHandler)
    {
        this->ReleaseHandler(m_pHandler);
        m_pHandler = nullptr;
    }

    return SDP::CHtsClientService::OnExit();
}

void CMcsForwarderAgent::OnSdpMsg(const MQ_MSG::CMcsExitAgentNotify& notify)
{
    // SDP_MESSAGE_TRACE("MQ_MSG::CMcsExitAgentNotify.");

    if (m_forwarderIpAddr.GetIpAddr() != notify.m_strIpAddr || m_forwarderIpAddr.GetPort() != notify.m_nPort)
    {
        SDP_RUN_LOG_WARNING("IP address or port mismatch. Local IP: {}, Local Port: {}, Remote IP: {}, Remote Port: {}",
                            m_forwarderIpAddr.GetIpAddr().c_str(), m_forwarderIpAddr.GetPort(), notify.m_strIpAddr.c_str(), notify.m_nPort);
    }

    if (m_pHandler != nullptr)
    {
        const SDP::CSdpMsgAddr addr(MQ_ID::EN_MQ_CLIENT_SDK_SCHEDULER_SERVICE_TYPE, MQ_ID::EN_MQ_CLIENT_SDK_SCHEDULER_SERVICE_ID);
        MQ_MSG::CAgentNotReadyMsg msg = {};

        this->SendMsg(msg, addr);
        this->CancelConnect(m_forwarderIpAddr);
        m_pHandler = nullptr;
    }
}

bool CMcsForwarderAgent::OnMcpConnected(const SOS::handle_type handle, const std::uint32_t nHandleID, const SDP::CIpAddr& rRemoteAddr,
                                        const SDP::CIpAddr& localAddr, const SDP::PROT_TYPE nProtType)
{
    if (const auto pHandler = SDP_NEW(CMqForwarderAgentHandler, *this, handle, nHandleID, rRemoteAddr, localAddr, nProtType, CMqForwarderAgentHandler::k_DEFAULT_MSG_BUF_SIZE))
    {
        if (pHandler->RegisterHandler(SOS::EN_READ_MASK))
        {
			m_pHandler = pHandler;
			this->OnServiceNetworkError(pHandler);

            return false;
        }

        SDP_MESSAGE_TRACE("new connect to forwarder.");

        int nBufSize = 0;
        if (-1 == SDP::CFG::GetProfileInteger("mq", "socket_buffer_size", nBufSize))
        {
            SDP_RUN_LOG_INFO("Configuration item 'socket_buffer_size' not found. Skipping socket buffer size configuration.");
        }
        else
        {
            nBufSize *= 1024;
            SOS::SetSockOpt(pHandler->GetHandle(), SOL_SOCKET, SO_SNDBUF, &nBufSize, sizeof(nBufSize));
            SOS::SetSockOpt(pHandler->GetHandle(), SOL_SOCKET, SO_RCVBUF, &nBufSize, sizeof(nBufSize));
        }

        if (m_nTcpNodelayFlag)
        {
            constexpr int nodelay = 1;

            if (SOS::SetSockOpt(pHandler->GetHandle(), IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)))
            {
                SDP_LOG_LAST_ERROR("setsockopt() failed");
            }
        }

        SDP_ASSERT(m_pHandler == nullptr);
        m_pHandler = pHandler;

        if (SDP::SendStatus::EN_FAILED == this->SendHtsMsg(m_regMqTopicReq, pHandler))
        {
            this->OnServiceNetworkError(pHandler);
            SDP_RUN_LOG_WARNING("Failed to register MQ topic request.");
        }

        return true;
    }

    return true;
}

void CMcsForwarderAgent::OnServiceNetworkError(CMqForwarderAgentHandler* pHandler)
{
    SDP_RUN_LOG_WARNING("Network error occurred in service, addr: {}, port: {}, handle id: {}.",
                        pHandler->GetRemoteAddr().GetIpAddr().c_str(),
                        pHandler->GetRemoteAddr().GetPort(),
                        pHandler->GetHandleID());

    const SDP::CSdpMsgAddr addr(MQ_ID::EN_MQ_CLIENT_SDK_SCHEDULER_SERVICE_TYPE, MQ_ID::EN_MQ_CLIENT_SDK_SCHEDULER_SERVICE_ID);
    MQ_MSG::CAgentNotReadyMsg msg = {};

    this->SendMsg(msg, addr);

    SDP::CHtsClientService::OnClientNetworkError(pHandler);
    m_pHandler = nullptr;
}

int32_t CMcsForwarderAgent::OnHtsMsg(const MQ_MSG::CPubMqAddrMsg& rPubMqAddrMsg) const
{
    //SDP_MESSAGE_TRACE("forwarde addr msg, topic:{}", rPubMqAddrMsg.m_nTopicID);

#if CPP_STANDARD >= 20
    SDirectMsg addrMqMsg = {.m_nCorrID = this->GetTaskID(), .m_nClientID = rPubMqAddrMsg.m_nClientID, .m_nTopicID = rPubMqAddrMsg.m_nTopicID, .m_nMsgSize = rPubMqAddrMsg.m_msgBlob.GetSize(), .m_pMsg = rPubMqAddrMsg.m_msgBlob.GetBuffer()};
#else
	SDirectMsg addrMqMsg = {this->GetTaskID(), rPubMqAddrMsg.m_nClientID, rPubMqAddrMsg.m_nTopicID, rPubMqAddrMsg.m_msgBlob.GetSize(), rPubMqAddrMsg.m_msgBlob.GetBuffer()};
#endif

    m_pMqMsgCallback->m_pOnMqMsg(&addrMqMsg);

    return 0;
}

int32_t CMcsForwarderAgent::OnHtsMsg([[maybe_unused]] const MQ_MSG::CRegMqTopicRsp& rPubMqAddrMsg)
{
    const SDP::CSdpMsgAddr addr(MQ_ID::EN_MQ_CLIENT_SDK_SCHEDULER_SERVICE_TYPE, MQ_ID::EN_MQ_CLIENT_SDK_SCHEDULER_SERVICE_ID);
    MQ_MSG::CAgentReadyMsg msg = {};

    this->SendMsg(msg, addr);

    return 0;
}
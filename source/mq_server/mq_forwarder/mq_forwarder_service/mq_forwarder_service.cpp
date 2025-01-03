#include "sdp/pch.h"
#include "sdp/configure/configure.h"

#include "mq_forwarder_service/mq_forwarder_service.h"

CMqForwarderService::CMqForwarderServiceHandler::CMqForwarderServiceHandler(CMqForwarderService& rService, const SOS::handle_type handle, const std::uint32_t nHandleID, const std::uint32_t nMsgBufSize)
    : CMcpServerHandler(rService, handle, nHandleID, nMsgBufSize)
    , m_nClientID(0)
{
}

CMqForwarderService::CMqForwarderService(const std::uint32_t nTcpNodelayFlag)
    : m_nTcpNodelayFlag(nTcpNodelayFlag)
    , m_regMqTopicReq()
{
}

CMqForwarderService::~CMqForwarderService() = default;

std::int32_t CMqForwarderService::OnInit()
{
    if (SDP::CHtsServerService::OnInit())
    {
        SDP_RUN_LOG_ERROR("SDP::CHtsServerService::OnInit() failed!");

        return -1;
    }

    this->SetAffinity();

    return 0;
}

std::int32_t CMqForwarderService::OnExit()
{
    for (auto it = m_mapMqClientIDSession.begin(); it != m_mapMqClientIDSession.end();)
    {
        const auto pMqForwarderServiceHandler = it.second;

        ++it;
        this->ReleaseHandler(pMqForwarderServiceHandler);
    }

    for (auto it = m_mapTopicSession.begin(); it != m_mapTopicSession.end();)
    {
        const auto pHandlerArray = it.second;

        ++it;

        const std::uint32_t nArySize = pHandlerArray->Size();
        for (std::uint32_t i = 0; i < nArySize; ++i)
        {
            SDP_DELETE(pHandlerArray->GetAt(i));
        }

        SDP_DEALLOCATE(pHandlerArray);
    }

    return SDP::CHtsServerService::OnExit();
}

void CMqForwarderService::OnAccept(const SOS::handle_type handle)
{
    if (m_nTcpNodelayFlag)
    {
        constexpr std::int32_t nodelay = 1;

        SOS::SetSockOpt(handle, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
    }

    if (auto pHandler = SDP_NEW(CMqForwarderServiceHandler, *this, handle, this->AllocHandleID(), CMqForwarderServiceHandler::k_DEFAULT_MSG_BUF_SIZE))
    {
        if (pHandler->RegisterHandler(SOS::EN_READ_MASK))
        {
            this->ReleaseHandler(pHandler);
            pHandler = nullptr;
        }
        else
        {
            this->OnConnectedNotify(pHandler);
        }
    }
}

void CMqForwarderService::OnServiceNetworkError(CMqForwarderServiceHandler* pHandler)
{
    SDP_MESSAGE_TRACE("disconnect, client id:{}!", pHandler->m_nClientID);

    auto& lstHandlerArrayItem = pHandler->m_lstHandlerArrayItem;
    for (const auto pHandlerArrayItem : lstHandlerArrayItem)
    {
        if (const auto pHandlerArray = m_mapTopicSession.Find(pHandlerArrayItem->m_nTopicID))
        {
            pHandlerArray->Erase(pHandlerArrayItem);

            if (pHandlerArray->IsEmpty())
            {
                m_mapTopicSession.DirectErase(pHandlerArray);
                SDP_DELETE(pHandlerArray);
            }
        }

        SDP_DELETE(pHandlerArrayItem);
    }
    lstHandlerArrayItem.clear();

    if (pHandler->m_nClientID)
    {
        m_mapMqClientIDSession.Erase(pHandler->m_nClientID);
    }

    this->ReleaseHandler(pHandler);
}

void CMqForwarderService::OnSdpMsg(const MQ_MSG::CForwardRegMqTopicReq& req, [[maybe_unused]] const SDP::CSdpMsgAddr& rSrcMsgAddr)
{
    SDP_MESSAGE_TRACE("recv MQ_MSG::CRegMqTopicReq, fw id:{}", this->GetTaskID());

    m_regMqTopicReq = req.m_regMqTopicReq;

    if (CMqForwarderServiceHandler* pOldHandler = m_mapMqClientIDSession.Find(m_regMqTopicReq.m_nClientID))
    {
        if (pOldHandler->GetHandle() != req.m_handle)
        {
            SDP_RUN_LOG_INFO("kickoff old client");
            m_mapMqClientIDSession.DirectErase(pOldHandler);
            this->OnServiceNetworkError(pOldHandler); // kick off old one
        }
    }

    this->OnAccept(req.m_handle);
}

std::int32_t CMqForwarderService::OnHtsMsg(const MQ_MSG::CPubMqTopicMsg& rPubMqTopicMsg, const CMqForwarderServiceHandler* pHandler)
{
    SDP_MESSAGE_TRACE("recv MQ_MSG::CPubMqTopicMsg, topic id:{}, task id:{}", rPubMqTopicMsg.m_nTopicID, this->GetTaskID());

    if (const auto pHandlerArray = m_mapTopicSession.Find(rPubMqTopicMsg.m_nTopicID))
    {
        if (MQ_MSG::CPubMqAddrMsg addrMsg(pHandler->m_nClientID, rPubMqTopicMsg); SDP::SendStatus::EN_FAILED == this->SendHtsMsg(addrMsg, pHandlerArray->SeqGet()->m_pHandler))
        {
            this->OnServiceNetworkError(pHandlerArray->GetAt()->m_pHandler);
			SDP_RUN_LOG_INFO("Failed to forward topic message, topic ID: {}", rPubMqTopicMsg.m_nTopicID);
		}
    }
    else
    {
		SDP_RUN_LOG_INFO("Cannot find topic with ID: {}", rPubMqTopicMsg.m_nTopicID);
    }

    return 0;
}

std::int32_t CMqForwarderService::OnHtsMsg(MQ_MSG::CPubMqAddrMsg& rPubMqAddrMsg, const CMqForwarderServiceHandler* pHandler)
{
    SDP_MESSAGE_TRACE("recv MQ_MSG::CPubMqAddrMsg, client id:{}, task id:{}", rPubMqAddrMsg.m_nClientID, this->GetTaskID());

    if (CMqForwarderServiceHandler* pSubHandler = m_mapMqClientIDSession.Find(rPubMqAddrMsg.m_nClientID))
    {
        rPubMqAddrMsg.m_nClientID = pHandler->m_nClientID;
        if (SDP::SendStatus::EN_FAILED == this->SendHtsMsg(rPubMqAddrMsg, pSubHandler))
        {
            this->OnServiceNetworkError(pSubHandler);
			SDP_RUN_LOG_INFO("Failed to forward topic message, topic ID: {}", rPubMqAddrMsg.m_nTopicID);
		}
    }
    else
    {
		SDP_RUN_LOG_INFO("Cannot find topic with ID: {}", rPubMqAddrMsg.m_nTopicID);
	}

    return 0;
}

std::int32_t CMqForwarderService::OnHtsMsg(const MQ_MSG::CPubMqBroadcastMsg& rPubMqBroadcastMsg, const CMqForwarderServiceHandler* pHandler)
{
    SDP_MESSAGE_TRACE("forwarde broadcast msg, topic:{}", rPubMqBroadcastMsg.m_nTopicID);

    if (const auto pHandlerArray = m_mapTopicSession.Find(rPubMqBroadcastMsg.m_nTopicID))
    {
        MQ_MSG::CPubMqAddrMsg addrMsg(pHandler->m_nClientID, rPubMqBroadcastMsg);
        const uint32_t nSize = pHandlerArray->Size();

        for (uint32_t i = 0; i < nSize; ++i)
        {
            if (SDP::SendStatus::EN_FAILED == this->SendHtsMsg(addrMsg, pHandlerArray->GetAt(i)->m_pHandler))
            {
                this->OnServiceNetworkError(pHandlerArray->GetAt(i)->m_pHandler);
				SDP_RUN_LOG_INFO("Failed to forward topic message, topic ID: {}", rPubMqBroadcastMsg.m_nTopicID);
				break;
            }
        }
    }
    else
    {
		SDP_RUN_LOG_INFO("Cannot find topic with ID: {}", rPubMqBroadcastMsg.m_nTopicID);
	}

    return 0;
}

bool CMqForwarderService::OnConnectedNotify(CMqForwarderServiceHandler* pHandler)
{
    pHandler->m_nClientID = m_regMqTopicReq.m_nClientID;
    m_mapMqClientIDSession.DirectInsert(m_regMqTopicReq.m_nClientID, pHandler);

    for (const auto nTopicID : m_regMqTopicReq.m_lstTopicID)
    {
        auto pHandlerArray = m_mapTopicSession.Find(nTopicID);

        if (!pHandlerArray)
        {
            pHandlerArray = SDP_NEW(CForwarderHandlerArray);

            m_mapTopicSession.DirectInsert(nTopicID, pHandlerArray);
        }

        if (auto pHandlerArrayItem = SDP_NEW(CForwarderHandlerArrayItem))
        {
            pHandlerArrayItem->m_nTopicID = nTopicID;
            pHandlerArrayItem->m_pHandler = pHandler;

            pHandlerArray->Insert(pHandlerArrayItem);
            pHandler->m_lstHandlerArrayItem.push_front(pHandlerArrayItem);
        }
    }

    std::int32_t nBufSize = 0;
    if (-1 == SDP::CFG::GetProfileInteger("mq", "socket_buffer_size", nBufSize))
    {
		SDP_RUN_LOG_ERROR("Failed to read 'socket_buffer_size' configuration value. Please check the configuration file.");
    }
    else
    {
        nBufSize *= 1024;
        SOS::SetSockOpt(pHandler->GetHandle(), SOL_SOCKET, SO_SNDBUF, &nBufSize, sizeof(nBufSize));
        SOS::SetSockOpt(pHandler->GetHandle(), SOL_SOCKET, SO_RCVBUF, &nBufSize, sizeof(nBufSize));
    }

    MQ_MSG::CRegMqTopicRsp rsp = {m_regMqTopicReq.m_nClientID};

    this->SendHtsMsg(rsp, pHandler);

    return true;
}
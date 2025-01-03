inline int32_t CMcsForwarderAgent::CMqForwarderAgentHandler::OnGetMessage(char* pBuffer, const SOS::s_size_t nBufferLen, int32_t& nBytes)
{
    return this->GetService().OnHtsMessage(pBuffer, nBufferLen, nBytes, this);
}

template <typename TYPE>
void CMcsForwarderAgent::SendMqMsg2Server(TYPE& rMsg)
{
    if (m_pHandler)
    {
        if (SDP::SendStatus::EN_FAILED == this->SendHtsMsg(rMsg, m_pHandler))
        {
            this->OnServiceNetworkError(m_pHandler);
            m_pHandler = nullptr;
            SDP_RUN_LOG_INFO("Failed to forward message, topic: {}", rMsg.m_nTopicID);
        }
        else
        {
            // SDP_MESSAGE_TRACE("forward msg succeed, topic:{}, handle id:{}", rMsg.m_nTopicID, pHandler->GetHandleID());
        }
    }
    else
    {
        SDP_RUN_LOG_INFO("Forward array is empty.");
    }
}

template <typename TYPE>
inline void CMcsForwarderAgent::OnMqMsg(TYPE& rMqMsg)
{
    const SDP::CSdpMsgAddr addr(MQ_ID::EN_MQ_CLIENT_SDK_TASK_TYPE, MQ_ID::EN_MQ_CLIENT_SDK_TASK_ID);

    this->SendMsg(rMqMsg, addr);
}

//inline void CMcsForwarderAgent::CMqForwarderAgentHandler::HandleException()
//{
//	m_rService.OnServiceNetworkError(this);
//}

//inline bool CMcsForwarderAgent::CMqForwarderAgentHandler::OnConnectedNotify()
//{
//	return m_rService.OnConnectedNotify(this);
//}

//inline int32_t CMcsForwarderAgent::CMqForwarderAgentHandler::OnGetMessage(char* pBuffer, const SOS::s_size_t nBufferLen, int32_t& nBytes)
//{
//	return m_rService.OnHtsMessage(pBuffer, nBufferLen, nBytes, this);
//}


inline void CMcsForwarderAgent::OnSdpMsg(MQ_MSG::CPubMqTopicMsg& rMsg)
{
    //SDP_MESSAGE_TRACE("MQ_MSG::CPubMqTopicMsg, topic:{}", rMsg.m_nTopicID);

    this->SendMqMsg2Server(rMsg);
}

inline void CMcsForwarderAgent::OnSdpMsg(MQ_MSG::CPubMqAddrMsg& rMsg)
{
    //SDP_MESSAGE_TRACE("MQ_MSG::CPubMqAddrMsg, topic:{}", rMsg.m_nTopicID);

    this->SendMqMsg2Server(rMsg);
}

inline void CMcsForwarderAgent::OnSdpMsg(MQ_MSG::CPubMqBroadcastMsg& rMsg)
{
    //SDP_MESSAGE_TRACE("MQ_MSG::CPubMqBroadcastMsg, topic:{}", rMsg.m_nTopicID);

    this->SendMqMsg2Server(rMsg);
}
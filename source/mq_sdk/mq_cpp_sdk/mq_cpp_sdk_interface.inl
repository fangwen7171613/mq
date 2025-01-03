#include "sdp/codec/sdp_codec/sdp_codec.h"
#include "mq_c_sdk/mcs_interface.h"

template <typename TYPE>
inline MQ::CMqMessageEncoder<TYPE>::CMqMessageEncoder(TYPE& t)
    : m_pMsg(nullptr)
    , m_nMsgSize(0)
{
    SDP::SdpEncode(t, m_pMsg, m_nMsgSize);
}

template <typename TYPE>
inline MQ::CMqMessageEncoder<TYPE>::~CMqMessageEncoder()
{
    SDP_SIZE_FREE(m_pMsg, m_nMsgSize);
}

template <typename TYPE>
inline int MQ::PubTopicPollMqMsg(TYPE& t)
{
    MQ::CMqMessageEncoder<TYPE> encoder(t);
#if CPP_STANDARD >= 20
    STopicMsg topicMqMsg = {.m_nTopicID = TYPE::EN_TOPIC_ID, .m_nMsgSize = encoder.m_nMsgSize, .m_pMsg = encoder.m_pMsg};
#else
	STopicMsg topicMqMsg = {TYPE::EN_TOPIC_ID, encoder.m_nMsgSize, encoder.m_pMsg};
#endif

    return PubTopRebMqMsg(&topicMqMsg);
}

template <typename TYPE>
inline int MQ::PubTopicBroadcastMqMsg(TYPE& t)
{
    MQ::CMqMessageEncoder<TYPE> encoder(t);
#if CPP_STANDARD >= 20
    STopicMsg topicMqMsg = {.m_nTopicID = TYPE::EN_TOPIC_ID, .m_nMsgSize = encoder.m_nMsgSize, .m_pMsg = encoder.m_pMsg};
#else
	STopicMsg topicMqMsg = {TYPE::EN_TOPIC_ID, encoder.m_nMsgSize, encoder.m_pMsg};
#endif

    return PubTopBcstMqMsg(&topicMqMsg);
}

template <typename TYPE>
inline int MQ::PubDirMqMsg(TYPE& t, const unsigned int nClientID, const unsigned short nID)
{
    MQ::CMqMessageEncoder<TYPE> encoder(t);
#if CPP_STANDARD >= 20
    SDirectMsg addrMqMsg = {.m_nCorrID = nID, .m_nClientID = nClientID, .m_nTopicID = TYPE::EN_TOPIC_ID, .m_nMsgSize = encoder.m_nMsgSize, .m_pMsg = encoder.m_pMsg};
#else
	SDirectMsg addrMqMsg = {nID, nClientID, TYPE::EN_TOPIC_ID, encoder.m_nMsgSize, encoder.m_pMsg};
#endif

    return ::PubDirMqMsg(&addrMqMsg);
}
#include "sdp/codec/sdp_codec/sdp_codec.h"

template <typename TYPE>
int MQ::PubTopicPollMqMsg(TYPE& t)
{
	CTopicMqMsg topicMqMsg;

	topicMqMsg.m_nTopicID = TYPE::EN_TOPIC_ID;
	SDP::SdpEncode(t, topicMqMsg.m_pMsg, topicMqMsg.m_nMsgSize);

	int nRes = MQ::PubTopicPollMqMsg(topicMqMsg);

	SDP_SIZE_FREE(topicMqMsg.m_pMsg, topicMqMsg.m_nMsgSize);

	return nRes;
}

template <typename TYPE>
int MQ::PubTopicBroadcastMqMsg(TYPE& t)
{
	CTopicMqMsg topicMqMsg;

	topicMqMsg.m_nTopicID = TYPE::EN_TOPIC_ID;
	SDP::SdpEncode(t, topicMqMsg.m_pMsg, topicMqMsg.m_nMsgSize);

	int nRes = MQ::PubTopicBroadcastMqMsg(topicMqMsg);

	SDP_SIZE_FREE(topicMqMsg.m_pMsg, topicMqMsg.m_nMsgSize);

	return nRes;
}

template <typename TYPE>
int MQ::PubAddrMqMsg(const uint32_t nClientID, TYPE& t)
{
	CAddrMqMsg addrMqMsg;

	addrMqMsg.m_nClientID = nClientID;
	addrMqMsg.m_nTopicID = TYPE::EN_TOPIC_ID;
	SDP::SdpEncode(t, addrMqMsg.m_pMsg, addrMqMsg.m_nMsgSize);

	int nRes = MQ::PubAddrMqMsg(addrMqMsg);

	SDP_SIZE_FREE(addrMqMsg.m_pMsg, addrMqMsg.m_nMsgSize);

	return nRes;
}

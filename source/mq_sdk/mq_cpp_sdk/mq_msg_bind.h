#pragma once

#include "sdp/count_parms_macros.h"

#define PROC_MQ_MSG(T)                                                                                          \
	case T::EN_TOPIC_ID:                                                                                        \
	{                                                                                                           \
		T msg;                                                                                                  \
		if (SDP::SdpDecode(msg, pMqMsg->m_pMsg, pMqMsg->m_nMsgSize))                                            \
		{                                                                                                       \
			SDP_RUN_LOG_WARNING("decode  msg failed, msg type:{}", static_cast<std::uint16_t>(T::EN_TOPIC_ID)); \
		}                                                                                                       \
		else                                                                                                    \
		{                                                                                                       \
			OnMqMsg(msg, pMqMsg->m_nClientID, pMqMsg->m_nCorrID);                                               \
		}                                                                                                       \
		break;                                                                                                  \
	}

#define PROC_MQ_MSG_FOREACH(...) SDP_FOR_EACH(PROC_MQ_MSG, __VA_ARGS__)

#define BIND_MQ_MSG(...)                                                     \
	void OnMqMsg(SDirectMsg* pMqMsg) final                                   \
	{                                                                        \
		switch (pMqMsg->m_nTopicID)                                          \
		{                                                                    \
			PROC_MQ_MSG_FOREACH(__VA_ARGS__)                                 \
			default:                                                         \
			{                                                                \
				SDP_RUN_LOG_WARNING("unknown topic:{}", pMqMsg->m_nTopicID); \
				break;                                                       \
			}                                                                \
		}                                                                    \
	}
// end of file
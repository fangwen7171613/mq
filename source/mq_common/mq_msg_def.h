#pragma once

#include "sdp/codec/sdp_codec/stl_list_codec.h"
#include "sdp/codec/sdp_codec/blob_codec.h"

#include "mq_common/mq_id_def.h"

namespace MQ_MSG
{
    class CRegisterMqForwarderServiceReq
    {
    public:
        enum
        {
            EN_MSG_TYPE = MQ_ID::EN_REGIST_MQ_FORWARDER_SERVICE_MSG
        };

    public:
        SDP_BIND_ATTR(m_nPort, m_nForwarderNum, m_strAddr)

    public:
        std::uint16_t m_nPort;
        std::int32_t m_nForwarderNum;
        SDP::CSdpString m_strAddr;
    };

    class CRegMqClientAddrReq
    {
    public:
        enum
        {
            EN_MSG_TYPE = MQ_ID::EN_REGIST_MQ_CLIENT_REQ_MSG
        };

    public:
        SDP_BIND_ATTR(m_nClientID)

    public:
        std::uint32_t m_nClientID;
    };

    class CMqForwarderInfo
    {
    public:
        bool operator==(const CMqForwarderInfo& info) const
        {
            return m_nPort == info.m_nPort && m_strIpAddr == info.m_strIpAddr;
        }

        bool operator<(const CMqForwarderInfo& info) const
        {
            if (m_nPort == info.m_nPort)
            {
                return m_strIpAddr < info.m_strIpAddr;
            }

            return m_nPort < info.m_nPort;
        }

    public:
        SDP_BIND_ATTR(m_nPort, m_nForwarderNum, m_strIpAddr)

    public:
        std::uint16_t m_nPort;
        std::int32_t m_nForwarderNum;
        SDP::CSdpString m_strIpAddr;
    };

    class CRegMqClientAddrRsp
    {
    public:
        enum
        {
            EN_MSG_TYPE = MQ_ID::EN_REGIST_MQ_CLIENT_RSP_MSG
        };

    public:
        SDP_BIND_ATTR(m_nClientID, m_lstMqForwarderInfo)

    public:
        std::uint32_t m_nClientID;
        SDP::list_t<MQ_MSG::CMqForwarderInfo> m_lstMqForwarderInfo;
    };

    class CAddForwarderAddrNotify
    {
    public:
        enum
        {
            EN_MSG_TYPE = MQ_ID::EN_ADD_FOWRARDER_ADDR_NOTIFY_MSG
        };

    public:
        SDP_BIND_ATTR(m_mqForwarderInfo)

    public:
        MQ_MSG::CMqForwarderInfo m_mqForwarderInfo;
    };

    class CDelForwarderAddrNotify
    {
    public:
        enum
        {
            EN_MSG_TYPE = MQ_ID::EN_DEL_FOWRARDER_ADDR_NOTIFY_MSG
        };

    public:
        SDP_BIND_ATTR(m_mqForwarderInfo)

    public:
        MQ_MSG::CMqForwarderInfo m_mqForwarderInfo;
    };

    class CRegMqTopicReq
    {
    public:
        enum
        {
            EN_MSG_TYPE = MQ_ID::EN_REGIST_MQ_TOPIC_REQ_MSG
        };

    public:
        SDP_BIND_ATTR(m_nClientID, m_lstTopicID)

    public:
        std::uint32_t m_nClientID;
        SDP::list_t<std::uint32_t> m_lstTopicID;
    };

    class CRegMqTopicRsp
    {
    public:
        enum
        {
            EN_MSG_TYPE = MQ_ID::EN_REGIST_MQ_TOPIC_RSP_MSG
        };

    public:
        SDP_BIND_ATTR(m_nClientID)

    public:
        std::uint32_t m_nClientID;
    };

    class CForwardRegMqTopicReq
    {
    public:
        enum
        {
            EN_MSG_TYPE = MQ_ID::EN_FORWARD_REGIST_MQ_TOPIC_REQ_MSG
        };

    public:
        SDP_BIND_ATTR(m_handle, m_regMqTopicReq)

    public:
        SOS::handle_type m_handle = SDP_INVALID_HANDLE;
        MQ_MSG::CRegMqTopicReq m_regMqTopicReq;
    };

    class CPubMqTopicMsg
    {
    public:
        enum
        {
            EN_MSG_TYPE = MQ_ID::EN_PUB_MQ_TOPIC_MSG
        };

        CPubMqTopicMsg() = default;

        CPubMqTopicMsg(const unsigned int nTopicID, char* pBlob, const std::uint32_t nSize)
            : m_nTopicID(nTopicID)
            , m_msgBlob(pBlob, nSize)
        {
        }

    public:
        SDP_BIND_ATTR(m_nTopicID, m_msgBlob)

    public:
        std::uint32_t m_nTopicID;
        SDP::CSdpBlob m_msgBlob;
    };

    class CPubMqBroadcastMsg
    {
    public:
        enum
        {
            EN_MSG_TYPE = MQ_ID::EN_PUB_MQ_BROADCAST_MSG
        };

        CPubMqBroadcastMsg() = default;

        CPubMqBroadcastMsg(const unsigned int nTopicID, char* pBlob, const std::uint32_t nSize)
            : m_nTopicID(nTopicID)
            , m_msgBlob(pBlob, nSize)
        {
        }

    public:
        SDP_BIND_ATTR(m_nTopicID, m_msgBlob)

    public:
        std::uint32_t m_nTopicID;
        SDP::CSdpBlob m_msgBlob;
    };

    class CPubMqAddrMsg
    {
    public:
        enum
        {
            EN_MSG_TYPE = MQ_ID::EN_PUB_MQ_ADDR_MSG
        };

    public:
        CPubMqAddrMsg()
            : m_nClientID(0)
            , m_nTopicID(0)
        {
        }

        CPubMqAddrMsg(const unsigned int nClientID, const unsigned int nTopicID, char* pBlob, const std::uint32_t nSize)
            : m_nClientID(nClientID)
            , m_nTopicID(nTopicID)
            , m_msgBlob(pBlob, nSize)
        {
        }

        CPubMqAddrMsg(const std::uint32_t nClientID, const CPubMqTopicMsg& msg)
            : m_nClientID(nClientID)
            , m_nTopicID(msg.m_nTopicID)
            , m_msgBlob(msg.m_msgBlob)
        {
        }

        CPubMqAddrMsg(const std::uint32_t nClientID, const CPubMqAddrMsg& msg)
            : m_nClientID(nClientID)
            , m_nTopicID(msg.m_nTopicID)
            , m_msgBlob(msg.m_msgBlob)
        {
        }

        CPubMqAddrMsg(const std::uint32_t nClientID, const CPubMqBroadcastMsg& msg)
            : m_nClientID(nClientID)
            , m_nTopicID(msg.m_nTopicID)
            , m_msgBlob(msg.m_msgBlob)
        {
        }

    public:
        SDP_BIND_ATTR(m_nClientID, m_nTopicID, m_msgBlob)

    public:
        std::uint32_t m_nClientID;
        std::uint32_t m_nTopicID;
        SDP::CSdpBlob m_msgBlob;
    };

    class CMcsExitAgentNotify
    {
    public:
        enum
        {
            EN_MSG_TYPE = MQ_ID::EN_MCS_EXIT_AGENT_NOTIFY
        };

    public:
        SDP_BIND_ATTR(m_nPort, m_strIpAddr)

    public:
        std::uint16_t m_nPort;
        SDP::CSdpString m_strIpAddr;
    };

    class CAgentReadyMsg
    {
    public:
        enum
        {
            EN_MSG_TYPE = MQ_ID::EN_AGENT_READY_MSG
        };

    public:
        SDP_BIND_ATTR(m_nDummy)

    public:
        uint32_t m_nDummy;
    };

    class CAgentNotReadyMsg
    {
    public:
        enum
        {
            EN_MSG_TYPE = MQ_ID::EN_AGENT_NOT_READY_MSG
        };

    public:
        SDP_BIND_ATTR(m_nDummy)

    public:
        uint32_t m_nDummy;
    };
} // namespace MQ_MSG
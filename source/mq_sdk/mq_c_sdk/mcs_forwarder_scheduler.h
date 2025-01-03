#pragma once

#include "sdp/container/stl_type/stl_list.h"
#include "sdp/container/stl_type/stl_map.h"
#include "sdp/protocol/hts/hts_service.h"
#include "sdp/protocol/hts/hts_msg_bind_macros.h"
#include "sdp/protocol/mcp/mcp_client_service_handler.h"

#include "mq_common/mq_msg_def.h"
#include "mq_c_sdk/mcs_common_def.h"

class CMcsForwarderScheduler final : public SDP::CHtsClientService
{
    class CMqSchedulerHandler final : public SDP::CMcpClientHandler<CMcsForwarderScheduler>
    {
    public:
        static constexpr std::size_t k_DEFAULT_MSG_BUF_SIZE = 256 * 1024;

        CMqSchedulerHandler(CMcsForwarderScheduler& rService, SOS::handle_type handle, uint32_t nHandleID,
                            const SDP::CIpAddr& rRemoteAddr, const SDP::CIpAddr& localAddr, SDP::PROT_TYPE nProtType, std::uint32_t nMsgBufSize);

    private:
        void HandleException() override;
        int32_t OnGetMessage(char* pBuffer, SOS::s_size_t nBufferLen, int32_t& nBytes) override;
    };

    class CFwdInfo
    {
    public:
        std::int32_t m_nForwarderNum;
        std::uint16_t m_nAgtID;
    };

public:
    static constexpr std::uint16_t EN_SERVICE_TYPE = MQ_ID::EN_MQ_CLIENT_SDK_SCHEDULER_SERVICE_TYPE;
    static constexpr std::uint16_t EN_SERVICE_ID = MQ_ID::EN_MQ_CLIENT_SDK_SCHEDULER_SERVICE_ID;

    CMcsForwarderScheduler(const MqMsgCb* pMqMsgCallback, std::uint32_t nClientID, const SDP::list_t<std::uint32_t>& lstTopicID);
    ~CMcsForwarderScheduler() override;

    std::int32_t OnInit() override;
    std::int32_t OnExit() override;

    BIND_SDP_MSG(SDP::CHtsClientService, MQ_MSG::CAgentReadyMsg, MQ_MSG::CAgentNotReadyMsg)

    void OnSdpMsg(const MQ_MSG::CAgentReadyMsg& rMsg);
    void OnSdpMsg(const MQ_MSG::CAgentNotReadyMsg& rMsg);

    BING_HTS_MSG(MQ_MSG::CRegMqClientAddrRsp, MQ_MSG::CAddForwarderAddrNotify, MQ_MSG::CDelForwarderAddrNotify)

    std::int32_t OnHtsMsg(const MQ_MSG::CRegMqClientAddrRsp& rRegMqClientAddrRsp);
    std::int32_t OnHtsMsg(const MQ_MSG::CAddForwarderAddrNotify& rAddForwarderAddrNotify);
    std::int32_t OnHtsMsg(const MQ_MSG::CDelForwarderAddrNotify& rDelForwarderAddrNotify);

    void OnServiceNetworkError(CMqSchedulerHandler* pHandler);

protected:
    bool OnMcpConnected(SOS::handle_type handle, std::uint32_t nHandleID, const SDP::CIpAddr& rRemoteAddr,
                        const SDP::CIpAddr& localAddr, SDP::PROT_TYPE nProtType) override;

private:
	void AddMqForwarderAddr(const MQ_MSG::CMqForwarderInfo& fwInfo);

    std::uint16_t m_nAllocID;
    std::uint32_t m_nMqForwardNum;
    std::uint32_t m_nMcsReadyNum;
    const MqMsgCb* m_pMqMsgCallback;
    CMqSchedulerHandler* m_pHandler;
    MQ_MSG::CRegMqTopicReq m_regMqTopicReq;
    SDP::map_t<SDP::CIpAddr, CFwdInfo> m_mapFwdInfo;
};

#include "mq_c_sdk/mcs_forwarder_scheduler.inl"
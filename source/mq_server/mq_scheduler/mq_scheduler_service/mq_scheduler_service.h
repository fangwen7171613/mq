#pragma once

#include "sdp/container/stl_type/stl_set.h"
#include "sdp/container/stl_type/stl_map.h"
#include "sdp/conn_mgr/prot_asynch_acceptor.h"
#include "sdp/protocol/hts/hts_service.h"
#include "sdp/protocol/hts/hts_msg_bind_macros.h"
#include "sdp/protocol/mcp/mcp_server_handler.h"

#include "mq_common/mq_msg_def.h"

class CMqSchedulerService final : public SDP::CHtsServerService
{
    class CForwarderAddrSession
    {
    public:
        CForwarderAddrSession(uint16_t nNodeType, uint16_t nNodeID, int32_t nForwarderNum, uint16_t nPort, const SDP::CSdpString& strAddr);

        bool operator<(const CForwarderAddrSession& rForwarderAddrSession) const;

        const uint16_t m_nNodeType;
        const uint16_t m_nNodeID;
        const int32_t m_nForwarderNum;
        const uint16_t m_nPort;
        const SDP::CSdpString m_strAddr;
    };

    class CMqSchedulerServiceHandler final : public SDP::CMcpServerHandler<CMqSchedulerService>
    {
    public:
        static constexpr std::size_t k_DEFAULT_MSG_BUF_SIZE = 256 * 1024;

        CMqSchedulerServiceHandler(CMqSchedulerService& rService, SOS::handle_type handle, uint32_t nHandleID, std::uint32_t nMsgBufSize);

    private:
        void HandleException() override;
        int32_t OnGetMessage(char* pBuffer, SOS::s_size_t nBufferLen, int32_t& nBytes) override;

    public:
        uint32_t m_nClientID;
    };

public:
    static constexpr std::uint16_t EN_SERVICE_TYPE = MQ_ID::EN_MQ_SCHEDULER_SERVICE_TYPE;
    static constexpr std::uint16_t EN_SERVICE_ID = MQ_ID::EN_MQ_SCHEDULER_SERVICE_ID;

    CMqSchedulerService();

    int32_t OnInit() override;
    int32_t OnExit() override;
    void OnAccept(SOS::handle_type handle) override;

    BIND_SDP_MSG(SDP::CHtsServerService, MQ_MSG::CRegisterMqForwarderServiceReq)

    BING_HTS_MSG(MQ_MSG::CRegMqClientAddrReq)

    void OnSdpMsg(const MQ_MSG::CRegisterMqForwarderServiceReq& req, const SDP::CSdpMsgAddr& rSrcMsgAddr);

    int32_t OnHtsMsg(MQ_MSG::CRegMqClientAddrReq& rRegMqClientAddrReq, CMqSchedulerServiceHandler* pHandler);

    void OnServiceNetworkError(CMqSchedulerServiceHandler* pHandler);
    void OnDisConnected(const SDP::CConnectInfo& rNetInfo) override;

private:
    SDP::CProcAsyncAcceptor<CMqSchedulerService> m_mqSchedulerServiceAcceptor;
    SDP::set_t<CForwarderAddrSession> m_setForwarderInfo;
    SDP::map_t<uint32_t, CMqSchedulerServiceHandler*> m_mapMqClientHandlerID;
};
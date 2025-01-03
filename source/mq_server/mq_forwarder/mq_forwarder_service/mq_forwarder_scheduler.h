#pragma once

#include "sdp/conn_mgr/prot_asynch_acceptor.h"
#include "sdp/protocol/hts/hts_service.h"
#include "sdp/protocol/hts/hts_msg_bind_macros.h"
#include "sdp/protocol/mcp/mcp_server_handler.h"

#include "mq_common/mq_msg_def.h"

class CMqForwarderScheduler final : public SDP::CHtsServerService
{
	class CMqForwarderSchedulerHandler final : public SDP::CMcpServerHandler<CMqForwarderScheduler>
	{
	public:
		static constexpr std::size_t k_DEFAULT_MSG_BUF_SIZE = 256 * 1024;

		CMqForwarderSchedulerHandler(CMqForwarderScheduler& rScheduler, SOS::handle_type handle, std::uint32_t nHandleID, std::uint32_t nMsgBufSize);

	private:
		int32_t OnGetMessage(char* pBuffer, SOS::s_size_t nBufferLen, int32_t& nBytes) override;
		void HandleException() override;
	};

public:
	static constexpr std::uint16_t EN_SERVICE_TYPE = MQ_ID::EN_MQ_SCHEDULER_AGENT_TYPE;
	static constexpr std::uint16_t EN_SERVICE_ID = MQ_ID::EN_MQ_SCHEDULER_AGENT_ID;

	explicit CMqForwarderScheduler(std::int32_t nMqForwardNum);

	std::int32_t OnInit() override;
	std::int32_t OnExit() override;
	void OnAccept(SOS::handle_type handle) override;

	BING_HTS_MSG(MQ_MSG::CRegMqTopicReq)

	std::int32_t OnHtsMsg(const MQ_MSG::CRegMqTopicReq& rRegMqTopicReq, CMqForwarderSchedulerHandler* pHandler);

	void OnConnected(const SDP::CConnectInfo& rNetInfo) override;

	void OnServiceNetworkError(CMqForwarderSchedulerHandler* pHandler) const;

private:
	std::int32_t m_nMqForwarderPort;
	const std::int32_t m_nMqForwardNum;
	std::map<std::uint32_t, std::uint16_t> m_mapCltID2FwdID; // 保存一份clientID 分配到forwarder的ID
	SDP::CSdpString m_strMqForwarderAddr;
	SDP::CProcAsyncAcceptor<CMqForwarderScheduler> m_mqAcceptor;
};
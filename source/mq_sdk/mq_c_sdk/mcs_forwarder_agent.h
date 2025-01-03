#pragma once

#include "sdp/protocol/hts/hts_service.h"
#include "sdp/protocol/hts/hts_msg_bind_macros.h"

#include "mq_common/mq_msg_def.h"
#include "mq_c_sdk/mcs_common_def.h"

class CMcsForwarderAgent final : public SDP::CHtsClientService
{
	class CMqForwarderAgentHandler final : public SDP::CMcpClientHandler<CMcsForwarderAgent>
	{
	public:
		static constexpr std::size_t k_DEFAULT_MSG_BUF_SIZE = 256 * 1024;

		CMqForwarderAgentHandler(CMcsForwarderAgent& rService, SOS::handle_type handle, uint32_t nHandleID,
								 const SDP::CIpAddr& rRemoteAddr, const SDP::CIpAddr& localAddr, SDP::PROT_TYPE nProtType, std::uint32_t nMsgBufSize);

	private:
		void HandleException() override;
		int32_t OnGetMessage(char* pBuffer, SOS::s_size_t nBufferLen, int32_t& nBytes) override;
	};

public:
	static constexpr std::uint16_t EN_SERVICE_TYPE = MQ_ID::EN_MQ_CLIENT_SDK_AGENT_SERVICE_TYPE;
	static constexpr std::uint16_t EN_SERVICE_ID = MQ_ID::EN_MQ_CLIENT_SDK_AGENT_SERVICE_ID;

	CMcsForwarderAgent(const MqMsgCb* pMqMsgCallback, uint32_t nClientID, const SDP::list_t<uint32_t>& lstTopicID, const SDP::CIpAddr& forwarderIpAddr);
	~CMcsForwarderAgent() override;

	int32_t OnInit() override;
	int32_t OnExit() override;

	BIND_SDP_MSG(SDP::CHtsClientService, MQ_MSG::CPubMqAddrMsg, MQ_MSG::CPubMqTopicMsg, MQ_MSG::CPubMqBroadcastMsg, MQ_MSG::CMcsExitAgentNotify)

	void OnSdpMsg(MQ_MSG::CPubMqTopicMsg& rMsg);
	void OnSdpMsg(MQ_MSG::CPubMqAddrMsg& rMsg);
	void OnSdpMsg(MQ_MSG::CPubMqBroadcastMsg& rMsg);
	void OnSdpMsg(const MQ_MSG::CMcsExitAgentNotify& notify);

	BING_HTS_MSG(MQ_MSG::CPubMqAddrMsg, MQ_MSG::CRegMqTopicRsp)

	int32_t OnHtsMsg(const MQ_MSG::CPubMqAddrMsg& rPubMqAddrMsg) const;
	int32_t OnHtsMsg(const MQ_MSG::CRegMqTopicRsp& rPubMqAddrMsg);

	void OnServiceNetworkError(CMqForwarderAgentHandler* pHandler);

protected:
	bool OnMcpConnected(SOS::handle_type handle, std::uint32_t nHandleID, const SDP::CIpAddr& rRemoteAddr,
						const SDP::CIpAddr& localAddr, SDP::PROT_TYPE nProtType) override;

private:
	template <typename TYPE>
	void SendMqMsg2Server(TYPE& rMsg);

	template <typename TYPE>
	void OnMqMsg(TYPE& rMqMsg);

	int32_t m_nTcpNodelayFlag;
	const MqMsgCb* m_pMqMsgCallback;
	CMqForwarderAgentHandler* m_pHandler;
	MQ_MSG::CRegMqTopicReq m_regMqTopicReq;
	SDP::CIpAddr m_forwarderIpAddr;
};

#include "mq_c_sdk/mcs_forwarder_agent.inl"
#pragma once

#include "sdp/container/stl_type/stl_forward_list.h"
#include "sdp/protocol/hts/hts_service.h"
#include "sdp/protocol/hts/hts_msg_bind_macros.h"
#include "sdp/protocol/mcp/mcp_server_handler.h"
#include "sdp/container/intrusive_hash_map.h"
#include "sdp/container/seq_get_type_array.h"

#include "mq_common/mq_msg_def.h"

class CMqForwarderService final : public SDP::CHtsServerService
{
	static constexpr std::uint32_t EN_TOPIC_ID_HASH_MAP_BITS = 16;
	static constexpr std::uint32_t EN_CLIENT_ID_HASH_MAP_BITS = 10;

	class CForwarderHandlerArrayItem;

	class CMqForwarderServiceHandler final : public SDP::CMcpServerHandler<CMqForwarderService>
	{
	public:
		static constexpr std::size_t k_DEFAULT_MSG_BUF_SIZE = 4 ^ 1024 * 1024;

		CMqForwarderServiceHandler(CMqForwarderService& rService, SOS::handle_type handle, std::uint32_t nHandleID, std::uint32_t nMsgBufSize);

		int32_t OnGetMessage(char* pBuffer, SOS::s_size_t nBufferLen, int32_t& nBytes) override;
		void HandleException() override;

		std::uint32_t m_nClientID;

		std::uint32_t m_nKey = 0;
		CMqForwarderServiceHandler* m_pPrev = nullptr;
		CMqForwarderServiceHandler* m_pNext = nullptr;

		SDP::forward_list_t<CForwarderHandlerArrayItem*> m_lstHandlerArrayItem;
	};

	class CForwarderHandlerArrayItem
	{
	public:
		std::uint32_t m_nIndex = 0XFFFFFFFF;
		std::uint32_t m_nTopicID;
		CMqForwarderServiceHandler* m_pHandler;
	};

	class CForwarderHandlerArray : public SDP::CSeqGetTypeArray<CForwarderHandlerArrayItem>
	{
	public:
		std::uint32_t m_nKey = 0;
		CForwarderHandlerArray* m_pPrev = nullptr;
		CForwarderHandlerArray* m_pNext = nullptr;
	};

public:
	static constexpr std::uint16_t EN_SERVICE_TYPE = MQ_ID::EN_MQ_FORWARDER_SERVICE_TYPE;

	explicit CMqForwarderService(std::uint32_t nTcpNodelayFlag);
	~CMqForwarderService() override;

	std::int32_t OnInit() override;
	std::int32_t OnExit() override;
	void OnAccept(SOS::handle_type handle) override;

	BIND_SDP_MSG(SDP::CHtsServerService, MQ_MSG::CForwardRegMqTopicReq)

	void OnSdpMsg(const MQ_MSG::CForwardRegMqTopicReq& req, const SDP::CSdpMsgAddr& rSrcMsgAddr);

	BING_HTS_MSG(MQ_MSG::CPubMqAddrMsg, MQ_MSG::CPubMqTopicMsg, MQ_MSG::CPubMqBroadcastMsg)

	std::int32_t OnHtsMsg(const MQ_MSG::CPubMqTopicMsg& rPubMqTopicMsg, const CMqForwarderServiceHandler* pHandler);
	std::int32_t OnHtsMsg(MQ_MSG::CPubMqAddrMsg& rPubMqAddrMsg, const CMqForwarderServiceHandler* pHandler);
	std::int32_t OnHtsMsg(const MQ_MSG::CPubMqBroadcastMsg& rPubMqBroadcastMsg, const CMqForwarderServiceHandler* pHandler);

	bool OnConnectedNotify(CMqForwarderServiceHandler* pHandler);
	void OnServiceNetworkError(CMqForwarderServiceHandler* pHandler);

private:
	const std::uint32_t m_nTcpNodelayFlag;

	using topic_id_map_type = SDP::CIntrusiveHashMap<std::uint32_t, CForwarderHandlerArray, EN_TOPIC_ID_HASH_MAP_BITS>;
	using client_id_map_type = SDP::CIntrusiveHashMap<std::uint32_t, CMqForwarderServiceHandler, EN_CLIENT_ID_HASH_MAP_BITS>;

	topic_id_map_type m_mapTopicSession;	   // mq topic id 2 sub array
	client_id_map_type m_mapMqClientIDSession; // mq client id 2 andler map
	MQ_MSG::CRegMqTopicReq m_regMqTopicReq;
};

#include "mq_forwarder_service/mq_forwarder_service.inl"
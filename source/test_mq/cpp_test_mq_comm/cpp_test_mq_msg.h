#pragma once

#include "sdp/codec/sdp_codec/stl_list_codec.h"
#include "sdp/codec/sdp_codec/datetime_codec.h"
#include "sdp/codec/sdp_codec/opt_type_codec.h"
#include "sdp/codec/sdp_codec/sdp_codec.h"

class CTestNestMqMsg
{
public:
	SDP_BIND_ATTR(m_nIntAttr, m_strAttr, m_datetimeAttr, m_optAttrl)

public:
	uint32_t m_nIntAttr;
	SDP::CSdpString m_strAttr;
	SDP::CDateTime m_datetimeAttr;
	SDP::COptType<SDP::CSdpString> m_optAttrl;
};

class CTestMqMsgReq1
{
public:
	enum
	{
		EN_TOPIC_ID = 1
	};

public:
	SDP_BIND_ATTR(m_nIntAttr, m_strAttr, m_datetimeAttr, m_optAttrl, m_lstAttr)

public:
	uint32_t m_nIntAttr;
	SDP::CSdpString m_strAttr;
	SDP::CDateTime m_datetimeAttr;
	SDP::COptType<SDP::CSdpString> m_optAttrl;
	SDP::list_t<CTestNestMqMsg> m_lstAttr;
};

class CTestMqMsgRsp1
{
public:
	enum
	{
		EN_TOPIC_ID = 2
	};

public:
	SDP_BIND_ATTR(m_nIntAttr, m_strAttr, m_datetimeAttr, m_optAttrl)

public:
	uint32_t m_nIntAttr;
	SDP::CSdpString m_strAttr;
	SDP::CDateTime m_datetimeAttr;
	SDP::COptType<SDP::CSdpString> m_optAttrl;
	SDP::list_t<CTestNestMqMsg> m_lstAttr;
};

class CTestMqMsgReq2
{
public:
	enum
	{
		EN_TOPIC_ID = 11
	};

public:
	SDP_BIND_ATTR(m_nIntAttr, m_strAttr, m_datetimeAttr, m_optAttrl, m_lstAttr)

public:
	uint32_t m_nIntAttr;
	SDP::CSdpString m_strAttr;
	SDP::CDateTime m_datetimeAttr;
	SDP::COptType<SDP::CSdpString> m_optAttrl;
	SDP::list_t<CTestNestMqMsg> m_lstAttr;
};

class CTestMqMsgRsp2
{
public:
	enum
	{
		EN_TOPIC_ID = 22
	};

public:
	SDP_BIND_ATTR(m_nIntAttr, m_strAttr, m_datetimeAttr, m_optAttrl, m_lstAttr)

public:
	uint32_t m_nIntAttr;
	SDP::CSdpString m_strAttr;
	SDP::CDateTime m_datetimeAttr;
	SDP::COptType<SDP::CSdpString> m_optAttrl;
	SDP::list_t<CTestNestMqMsg> m_lstAttr;
};
//==========================================================================================================
class CTestMqMsgReq3
{
public:
	enum
	{
		EN_TOPIC_ID = 3
	};

public:
	SDP_BIND_ATTR(m_nIntAttr)

public:
	uint32_t m_nIntAttr;
};

class CTestMqMsgRsp3
{
public:
	enum
	{
		EN_TOPIC_ID = 4
	};

public:
	SDP_BIND_ATTR(m_nIntAttr)

public:
	uint32_t m_nIntAttr;
};

class CTestMqMsgReq4
{
public:
	enum
	{
		EN_TOPIC_ID = 33
	};

public:
	SDP_BIND_ATTR(m_nIntAttr)

public:
	uint32_t m_nIntAttr;
};

class CTestMqMsgRsp4
{
public:
	enum
	{
		EN_TOPIC_ID = 44
	};

public:
	SDP_BIND_ATTR(m_nIntAttr)

public:
	uint32_t m_nIntAttr;
};
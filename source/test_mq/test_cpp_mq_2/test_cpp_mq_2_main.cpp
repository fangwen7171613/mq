#include "sdp/pch.h"
#ifdef WIN32
#include <windows.h>
#endif

#include "mq_cpp_sdk/mq_msg_bind.h"
#include "mq_cpp_sdk/mq_cpp_sdk_interface.h"
#include "mq_cpp_sdk/mq_msg_bind.h"
#include "cpp_test_mq_comm/cpp_test_mq_msg.h"

// class CTestMqMsgCallback : public MQ::IMqMsgProc
//{
// public:
//	void OnMqReady() final;
//	void OnMqNotReady() final;
//	void OnMqExit() final;
//
// public:
//	BIND_MQ_MSG(CTestMqMsgReq1, CTestMqMsgReq2)
//
// public:
//	void OnMqMsg(CTestMqMsgReq1& req, const unsigned int nClientID);
//	void OnMqMsg(CTestMqMsgReq2& req, const unsigned int nClientID);
// };

class CTestMqMsgCallback final : public MQ::IMqMsgProc
{
public:
    void OnMqReady() override;
    void OnMqNotReady() override;
    void OnMqExit() override;

public:
    BIND_MQ_MSG(CTestMqMsgReq3, CTestMqMsgReq4)

public:
    void OnMqMsg(const CTestMqMsgReq3& req, const unsigned int nClientID, unsigned short nID);
    void OnMqMsg(const CTestMqMsgReq4& req, const unsigned int nClientID, unsigned short nID);
};

//unsigned int g_msgNum = 0;
bool g_mqExited = false;
CTestMqMsgCallback g_TestMqMsgCallback;

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

#if CPP_STANDARD >= 20
    MQ::CInitMqCppSdkParam param = {
        .m_pMqMsgProc = &g_TestMqMsgCallback,
        .m_pszWorkRoot = nullptr,
        .m_pszModuleName = "test_cpp_mq_2",
        .m_vecTopicID{},
        .m_nClientID = 2};
#else
	MQ::CInitMqCppSdkParam param = {
		&g_TestMqMsgCallback,
		nullptr,
		"test_cpp_mq_2",
		{},
		2};
#endif
    param.m_vecTopicID.push_back(CTestMqMsgReq3::EN_TOPIC_ID);
    param.m_vecTopicID.push_back(CTestMqMsgReq4::EN_TOPIC_ID);

    if (MQ::InitMqCppSdk(param))
    {
        printf("Init Mq Cpp Sdk failed\n");

        return -1;
    }

    for (;;)
    {
        // printf("msg num:{}\n", g_msgNum);
        //g_msgNum = 0;
#ifdef WIN32
        Sleep(1000);
#else
		sleep(1);
#endif
        if (g_mqExited)
        {
            break;
        }
    }

    MQ::ExitMqCppSdk();

    printf("test_mq_2 exit\n");

    return 0;
}

void CTestMqMsgCallback::OnMqReady()
{
}

void CTestMqMsgCallback::OnMqNotReady()
{
}

void CTestMqMsgCallback::OnMqExit()
{
    ExitMqSdk();
    g_mqExited = true;
}

// void CTestMqMsgCallback::OnMqMsg(CTestMqMsgReq& req, const unsigned int nClientID)
//{
//	++g_msgNum;
//
//	CTestNestMqMsg testNextMqMsg1;
//	CTestMqMsgRsp1 testMqMsgRsp1;
//
//	testMqMsgRsp1.m_nIntAttr = 123;
//	testMqMsgRsp1.m_strAttr = "456";
//	SDP::CDateTime::GetDateTimeFromSecs(testMqMsgRsp1.m_datetimeAttr, time(nullptr));
//	*testMqMsgRsp1.m_optAttrl = "789";
//	testMqMsgRsp1.m_optAttrl.Exist(true);
//
//	testNextMqMsg1.m_nIntAttr = 123;
//	testNextMqMsg1.m_strAttr = "456";
//	SDP::CDateTime::GetDateTimeFromSecs(testNextMqMsg1.m_datetimeAttr, time(nullptr));
//	*testNextMqMsg1.m_optAttrl = "789";
//	testNextMqMsg1.m_optAttrl.Exist(true);
//
//	testMqMsgRsp1.m_lstAttr.push_back(testNextMqMsg1);
//
//	MQ::PubAddrMqMsg(nClientID, testMqMsgRsp1);
// }
//
// void CTestMqMsgCallback::OnMqMsg(CTestMqMsgReq2& req, const unsigned int nClientID)
//{
//	++g_msgNum;
//
//	CTestNestMqMsg testNextMqMsg1;
//	CTestMqMsgRsp2 testMqMsgRsp2;
//
//	testNextMqMsg1.m_nIntAttr = 123;
//	testNextMqMsg1.m_strAttr = "456";
//	SDP::CDateTime::GetDateTimeFromSecs(testNextMqMsg1.m_datetimeAttr, time(nullptr));
//	*testNextMqMsg1.m_optAttrl = "789";
//	testNextMqMsg1.m_optAttrl.Exist(true);
//
//	testMqMsgRsp2.m_nIntAttr = 123;
//	testMqMsgRsp2.m_strAttr = "456";
//	SDP::CDateTime::GetDateTimeFromSecs(testMqMsgRsp2.m_datetimeAttr, time(nullptr));
//	*testMqMsgRsp2.m_optAttrl = "789";
//	testMqMsgRsp2.m_optAttrl.Exist(true);
//	testMqMsgRsp2.m_lstAttr.push_back(testNextMqMsg1);
//
//	MQ::PubAddrMqMsg(nClientID, testMqMsgRsp2);
// }

void CTestMqMsgCallback::OnMqMsg([[maybe_unused]] const CTestMqMsgReq3& req, const unsigned int nClientID, unsigned short nID)
{
    //++g_msgNum;

    CTestMqMsgRsp3 testMqMsgRsp;

    testMqMsgRsp.m_nIntAttr = 123;
    MQ::PubDirMqMsg(testMqMsgRsp, nClientID, nID);
}

void CTestMqMsgCallback::OnMqMsg([[maybe_unused]] const CTestMqMsgReq4& req, const unsigned int nClientID, unsigned short nID)
{
    //++g_msgNum;

    CTestMqMsgRsp4 testMqMsgRsp;

    testMqMsgRsp.m_nIntAttr = 123;
    MQ::PubDirMqMsg(testMqMsgRsp, nClientID, nID);
}
#include "sdp/pch.h"

#include "mq_cpp_sdk/mq_cpp_sdk_interface.h"
#include "mq_cpp_sdk/mq_msg_bind.h"
#include "cpp_test_mq_comm/cpp_test_mq_msg.h"

// class CTestMqMsgCallback final : public MQ::IMqMsgProc
//{
// public:
//	void OnMqReady() final;
//	void OnMqNotReady() final;
//	void OnMqExit() final;
//
// public:
//	BIND_MQ_MSG(CTestMqMsgRsp1, CTestMqMsgRsp2)
//
// public:
//	void OnMqMsg(CTestMqMsgRsp1& rsp, const unsigned int nClientID);
//	void OnMqMsg(CTestMqMsgRsp2& rsp, const unsigned int nClientID);
// };

class CTestMqMsgCallback final : public MQ::IMqMsgProc
{
public:
    void OnMqReady() override;
    void OnMqNotReady() override;
    void OnMqExit() override;

public:
    BIND_MQ_MSG(CTestMqMsgRsp3, CTestMqMsgRsp4)

public:
    void OnMqMsg(const CTestMqMsgRsp3& rsp, const unsigned int nClientID, const unsigned short nID);
    void OnMqMsg(const CTestMqMsgRsp4& rsp, const unsigned int nClientID, const unsigned short nID);
};

unsigned int g_msgNum = 0;
bool g_mqExited = false;
CTestMqMsgCallback g_TestMqMsgCallback;

void SendReq3();
void SendReq4();

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
        .m_nClientID = 1};
#else
	MQ::CInitMqCppSdkParam param = {
		&g_TestMqMsgCallback,
		nullptr,
		"test_cpp_mq_2",
		{},
		1};
#endif
    param.m_vecTopicID.push_back(CTestMqMsgRsp3::EN_TOPIC_ID);
    param.m_vecTopicID.push_back(CTestMqMsgRsp4::EN_TOPIC_ID);

    if (MQ::InitMqCppSdk(param))
    {
        printf("Init Mq Cpp Sdk failed\n");

        return -1;
    }

    for (;;)
    {
        printf("msg num:%u\n", g_msgNum);
        g_msgNum = 0;
#ifdef WIN32
        Sleep(1000);
#else
		sleep(1);
#endif
        if (g_mqExited)
        {
            break;
        }

        //for (int i = 0; i < 200; ++i)
        //{
        //	if (g_msgNum == 0)
        //	{
        //		SendReq3();
        //		SendReq4();
        //	}
        //}
    }

    MQ::ExitMqCppSdk();

    printf("test_mq_1 exit\n");

    return 0;
}

void SendReq1()
{
    CTestNestMqMsg testNextMqMsg;
    CTestMqMsgReq1 testMqMsgReq1;

    testMqMsgReq1.m_nIntAttr = 123;
    testMqMsgReq1.m_strAttr = "456";
    SDP::CDateTime::GetDateTimeFromSecs(testMqMsgReq1.m_datetimeAttr, time(nullptr));
    *testMqMsgReq1.m_optAttrl = "789";
    testMqMsgReq1.m_optAttrl.Exist(true);

    testNextMqMsg.m_nIntAttr = 123;
    testNextMqMsg.m_strAttr = "456";
    SDP::CDateTime::GetDateTimeFromSecs(testNextMqMsg.m_datetimeAttr, time(nullptr));
    *testNextMqMsg.m_optAttrl = "789";
    testNextMqMsg.m_optAttrl.Exist(true);

    testMqMsgReq1.m_lstAttr.push_back(testNextMqMsg);

    MQ::PubTopicPollMqMsg(testMqMsgReq1);
}

void SendReq2()
{
    CTestNestMqMsg testNextMqMsg;
    CTestMqMsgReq2 testMqMsgReq2;

    testNextMqMsg.m_nIntAttr = 123;
    testNextMqMsg.m_strAttr = "456";
    SDP::CDateTime::GetDateTimeFromSecs(testNextMqMsg.m_datetimeAttr, time(nullptr));
    *testNextMqMsg.m_optAttrl = "789";
    testNextMqMsg.m_optAttrl.Exist(true);

    testMqMsgReq2.m_nIntAttr = 123;
    testMqMsgReq2.m_strAttr = "456";
    SDP::CDateTime::GetDateTimeFromSecs(testMqMsgReq2.m_datetimeAttr, time(nullptr));
    *testMqMsgReq2.m_optAttrl = "789";
    testMqMsgReq2.m_optAttrl.Exist(true);
    testMqMsgReq2.m_lstAttr.push_back(testNextMqMsg);

    MQ::PubTopicPollMqMsg(testMqMsgReq2);
}

void SendReq3()
{
#if CPP_STANDARD >= 20
    CTestMqMsgReq3 testMqMsgReq = {.m_nIntAttr = 123};
#else
	CTestMqMsgReq3 testMqMsgReq = {123};
#endif

    MQ::PubTopicPollMqMsg(testMqMsgReq);
}

void SendReq4()
{
#if CPP_STANDARD >= 20
    CTestMqMsgReq4 testMqMsgReq = {.m_nIntAttr = 123};
#else
	CTestMqMsgReq4 testMqMsgReq = {123};
#endif

    MQ::PubTopicPollMqMsg(testMqMsgReq);
}

void CTestMqMsgCallback::OnMqReady()
{
    for (int i = 0; i < 2000; ++i)
    {
        SendReq3();
        SendReq4();
    }
}

void CTestMqMsgCallback::OnMqNotReady()
{
}

void CTestMqMsgCallback::OnMqExit()
{
    ExitMqSdk();
    g_mqExited = true;
}

void CTestMqMsgCallback::OnMqMsg([[maybe_unused]] const CTestMqMsgRsp3& rsp, [[maybe_unused]] const unsigned int nClientID, const unsigned short nID)
{
    ++g_msgNum;
    CTestMqMsgReq3 testMqMsgReq = {123};

    MQ::PubDirMqMsg(testMqMsgReq, nClientID, nID);
}

void CTestMqMsgCallback::OnMqMsg([[maybe_unused]] const CTestMqMsgRsp4& rsp, [[maybe_unused]] const unsigned int nClientID, const unsigned short nID)
{
    ++g_msgNum;
    //printf("msg num:{}\n", g_msgNum);
#if CPP_STANDARD >= 20
    CTestMqMsgReq4 testMqMsgReq = {.m_nIntAttr = 123};
#else
	CTestMqMsgReq4 testMqMsgReq = {123};
#endif

    MQ::PubDirMqMsg(testMqMsgReq, nClientID, nID);
}
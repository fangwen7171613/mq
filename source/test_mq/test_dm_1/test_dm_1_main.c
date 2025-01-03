#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif //  _MSC_VER

#include <stdio.h>

#include "mq_c_sdk/mcs_interface.h"

void TestImCommEstablished(void);
void TestImCommInterrupted(void);
void TestMqExit(void);
void TestMqMsg(SDirectMsg* pMqMsg);

MqMsgCb g_testCallback = {
	.m_pOnMqCommEst = TestImCommEstablished,
	.m_pOnMqCommIntr = TestImCommInterrupted,
	.m_pOnMqMsg = TestMqMsg};

unsigned int g_msgNum = 0;
int g_mqExited = 0;

void PubTopicMsg(void);

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	const unsigned int topicList[2] = {6, 66};
	SInitMqSdkParam param = {.m_pMqMsgCb = &g_testCallback, TestMqExit, .m_pszWorkRoot = NULL, .m_pTopicArray = topicList, .m_nTopicCount = 2, .m_nClientID = 1};

	if (InitMqSdk(&param))
	{
		printf("Init Mq c Sdk failed\n");

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

		// SendReq();
	}

	printf("test_mq_1 exit\n");
	ExitMqSdk();

	return 0;
}

void PubTopicMsg(void)
{
	int i = 123;

	STopicMsg msg = {.m_nTopicID = 5, .m_nMsgSize = sizeof(int), .m_pMsg = (char*)&i};

	PubTopRebMqMsg(&msg);
}

void TestImCommEstablished(void)
{
	for (int i = 0; i < 2000; ++i)
	{
		PubTopicMsg();
	}
}

void TestImCommInterrupted(void)
{
}

void TestMqExit(void)
{
	// ExitMqSdk();
	g_mqExited = 1;
}

void TestMqMsg(SDirectMsg* pMqMsg)
{
	pMqMsg->m_nTopicID = 5;
	PubDirMqMsg(pMqMsg);
	++g_msgNum;
}
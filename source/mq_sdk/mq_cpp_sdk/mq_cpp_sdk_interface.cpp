#include "mq_cpp_sdk/mq_cpp_sdk_interface.h"

namespace
{
    MQ::IMqMsgProc* g_pMqMsgProc = nullptr;

    void CppMqReady()
    {
        if (g_pMqMsgProc)
        {
            g_pMqMsgProc->OnMqReady();
        }
    }

    void CppMqNotReady()
    {
        if (g_pMqMsgProc)
        {
            g_pMqMsgProc->OnMqNotReady();
        }
    }

    void CppMqMsg(SDirectMsg* pMqMsg)
    {
        if (g_pMqMsgProc)
        {
            g_pMqMsgProc->OnMqMsg(pMqMsg);
        }
    }

    MqMsgCb g_mqMsgCallback;
} // namespace

int MQ::InitMqCppSdk(const MQ::CInitMqCppSdkParam& param)
{
    SInitMqSdkParam initParam;

    if (!param.m_pMqMsgProc)
    {
        return -1;
    }

    initParam.m_pMqMsgCb = &g_mqMsgCallback;
    initParam.m_pszWorkRoot = param.m_pszWorkRoot;
    initParam.m_pTopicArray = param.m_vecTopicID.data();
    initParam.m_nTopicCount = static_cast<unsigned int>(param.m_vecTopicID.size());
    initParam.m_nClientID = param.m_nClientID;

    g_mqMsgCallback.m_pOnMqCommEst = CppMqReady;
    g_mqMsgCallback.m_pOnMqCommIntr = CppMqNotReady;
    g_mqMsgCallback.m_pOnMqMsg = CppMqMsg;

    g_pMqMsgProc = param.m_pMqMsgProc;

    const int nRet = InitMqSdk(&initParam);

    return nRet;
}

int MQ::ExitMqCppSdk()
{
    g_pMqMsgProc = nullptr;

    return ExitMqSdk();
}
#include "sdp/pch.h"
#include "sdp/sdk_comm/sdk_comm_init.h"
#include "sdp/snd_sdp_msg.h"
#include "sdp/configure/configure.h"
#include "sdp/dispatcher/dispatcher_center.h"
#include "mq_common/mq_msg_def.h"
#include "mq_c_sdk/mcs_forwarder_scheduler.h"
#include "mq_c_sdk/mcs_interface.h"

int InitMqSdk(const SInitMqSdkParam* pParam)
{
    if (nullptr == pParam)
    {
        SDP_STARTUP_TRACE("Parameter 'pParam' cannot be null.");

        return -1;
    }

    if (nullptr == pParam->m_pMqMsgCb)
    {
        SDP_STARTUP_TRACE("Parameter callback cannot be null.");

        return -1;
    }

    if (nullptr == pParam->m_pTopicArray)
    {
        SDP_STARTUP_TRACE("Topic list (m_pTopicList) cannot be null.");

        return -1;
    }

    if (-1 == SDP::InitSdk(pParam->m_pszWorkRoot, "mq_sdk", pParam->m_pMqMsgCb->m_OnMqExit))
    {
        SDP_STARTUP_TRACE("mq SDK initialization failed.");

        return -1;
    }

    SDP::list_t<std::uint32_t> lstTopicID;
    const unsigned int nTopicCount = pParam->m_nTopicCount;
    const unsigned int* pTopicList = pParam->m_pTopicArray;

    for (unsigned int i = 0; i < nTopicCount; ++i)
    {
        lstTopicID.push_front(pTopicList[i]);
    }

    const auto pScheduler = new(std::nothrow) CMcsForwarderScheduler(pParam->m_pMqMsgCb, pParam->m_nClientID, lstTopicID);

    if (!pScheduler)
    {
        SDP_STARTUP_TRACE("new  CMcsSchedulerService failed");

        return -1;
    }

    pScheduler->SetTaskType(CMcsForwarderScheduler::EN_SERVICE_TYPE);
    pScheduler->SetTaskID(CMcsForwarderScheduler::EN_SERVICE_ID);

    if (SDP::CDispatcherCenterDemon::Instance()->RegisterTask(pScheduler, CMcsForwarderScheduler::EN_SERVICE_TYPE, CMcsForwarderScheduler::EN_SERVICE_ID, true))
    {
        delete pScheduler;
        SDP_STARTUP_TRACE("mq client agent service registration failed");

        return -1;
    }

    SDP_STARTUP_TRACE("mq client SDK initialized successfully");

    return 0;
}

int ExitMqSdk()
{
    SDP::ExitSdk();

    return 0;
}

int PubTopRebMqMsg(const STopicMsg* pMsg)
{
    MQ_MSG::CPubMqTopicMsg msg(pMsg->m_nTopicID, const_cast<char*>(pMsg->m_pMsg), pMsg->m_nMsgSize);
    const SDP::CSdpMsgAddr addr(MQ_ID::EN_MQ_CLIENT_SDK_AGENT_SERVICE_TYPE, SDP::EN_INVALID_ID);

    return SendSdpMsg(msg, addr);
}

int PubTopBcstMqMsg(const STopicMsg* pMsg)
{
    MQ_MSG::CPubMqBroadcastMsg msg(pMsg->m_nTopicID, const_cast<char*>(pMsg->m_pMsg), pMsg->m_nMsgSize);
    const SDP::CSdpMsgAddr addr(MQ_ID::EN_MQ_CLIENT_SDK_AGENT_SERVICE_TYPE, SDP::EN_INVALID_ID);

    return SendSdpMsg(msg, addr);
}

int PubDirMqMsg(const SDirectMsg* pMsg)
{
    MQ_MSG::CPubMqAddrMsg msg(pMsg->m_nClientID, pMsg->m_nTopicID, const_cast<char*>(pMsg->m_pMsg), pMsg->m_nMsgSize);
    const SDP::CSdpMsgAddr addr(MQ_ID::EN_MQ_CLIENT_SDK_AGENT_SERVICE_TYPE, pMsg->m_nCorrID);

    return SendSdpMsg(msg, addr);
}
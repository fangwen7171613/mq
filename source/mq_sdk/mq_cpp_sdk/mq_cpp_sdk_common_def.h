#pragma once


#include "mq_c_sdk/mcs_common_def.h"
#include "mq_cpp_sdk/mq_cpp_sdk_export.h"

namespace MQ
{
    class MQ_CPP_EXPORT IMqMsgProc
    {
    public:
        virtual ~IMqMsgProc();

        virtual void OnMqReady() = 0;
        virtual void OnMqNotReady() = 0;
        virtual void OnMqExit() = 0;
        virtual void OnMqMsg(SDirectMsg* pMqMsg) = 0;
    };

    class CInitMqCppSdkParam
    {
    public:
        IMqMsgProc* m_pMqMsgProc;
        const char* m_pszWorkRoot;
        const char* m_pszModuleName;
        std::vector<std::uint32_t> m_vecTopicID;
        std::uint32_t m_nClientID;
    };
} // namespace MQ
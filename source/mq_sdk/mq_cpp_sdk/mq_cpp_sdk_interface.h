#pragma once


#include "mq_cpp_sdk/mq_cpp_sdk_export.h"
#include "mq_cpp_sdk/mq_cpp_sdk_common_def.h"

namespace MQ
{
    MQ_CPP_EXPORT int InitMqCppSdk(const MQ::CInitMqCppSdkParam& param);
    MQ_CPP_EXPORT int ExitMqCppSdk();

    template <typename TYPE>
    int PubTopicPollMqMsg(TYPE& t);

    template <typename TYPE>
    int PubTopicBroadcastMqMsg(TYPE& t);

    template <typename TYPE>
    int PubDirMqMsg(TYPE& t, unsigned int nClientID, unsigned short nID);

    template <typename TYPE>
    class CMqMessageEncoder
    {
    public:
        explicit CMqMessageEncoder(TYPE& t);
        ~CMqMessageEncoder();

        char* m_pMsg;
        uint32_t m_nMsgSize;
    };
} // namespace MQ

#include "mq_cpp_sdk/mq_cpp_sdk_interface.inl"
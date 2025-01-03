#ifndef MQ_CLIENT_SDK_INTERFACE_H
#define MQ_CLIENT_SDK_INTERFACE_H

#include "mq_c_sdk/mcs_export.h"
#include "mq_c_sdk/mcs_common_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

	// Initialize the Message Queue SDK
	// @param pParam - Pointer to the initialization parameters
	// @return int - Return status (0 for success, non-zero for failure)
	MQ_C_EXPORT int InitMqSdk(const SInitMqSdkParam* pParam);

	// Exit the Message Queue SDK
	// @return int - Return status (0 for success, non-zero for failure)
	MQ_C_EXPORT int ExitMqSdk(void);

	// Publish a topic-based message to a specific topic
	// @param pMsg - Pointer to the topic message to be published
	// @return int - Return status (0 for success, non-zero for failure)
	MQ_C_EXPORT int PubTopRebMqMsg(const STopicMsg* pMsg);

	// Publish a topic-based broadcast message to all subscribers of a topic
	// @param pMsg - Pointer to the topic message to be broadcast
	// @return int - Return status (0 for success, non-zero for failure)
	MQ_C_EXPORT int PubTopBcstMqMsg(const STopicMsg* pMsg);

	// Publish a direct message to a specific client or recipient
	// @param pMsg - Pointer to the direct message to be sent
	// @return int - Return status (0 for success, non-zero for failure)
	MQ_C_EXPORT int PubDirMqMsg(const SDirectMsg* pMsg);
    
#ifdef __cplusplus
}
#endif

#endif
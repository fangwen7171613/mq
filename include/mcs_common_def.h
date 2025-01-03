#ifndef MQ_CLIENT_SDK_COMMON_DEF_H
#define MQ_CLIENT_SDK_COMMON_DEF_H

#ifdef __cplusplus
extern "C"
{
#endif

// Define a structure for Topic Message
	struct tagTopicMsg
	{
		unsigned int m_nTopicID; // Topic ID
		unsigned int m_nMsgSize; // Message size
		const char* m_pMsg;		 // Message content
	};
	typedef struct tagTopicMsg STopicMsg;

	// Define a structure for Direct Message
	struct tagDirectMsg
	{
		unsigned short m_nCorrID; // Correlation ID, used to associate requests with responses
		unsigned int m_nClientID; // Client ID
		unsigned int m_nTopicID;  // Topic ID
		unsigned int m_nMsgSize;  // Message size
		const char* m_pMsg;		  // Message content
	};
	typedef struct tagDirectMsg SDirectMsg;

	// Define the callback function types for different communication events
	typedef void (*OnMqCommEst)(void);		   // Callback for immediate message communication establishment
	typedef void (*OnMqCommIntr)(void);		   // Callback for immediate message communication interruption
	typedef void (*OnMqMsg)(SDirectMsg* pMsg); // Callback for handling incoming messages
	typedef void (*OnMqExit)(void);			   // Callback for message queue exit

	// Define a structure for message queue callback functions
	struct tagMqMsgCb
	{
		OnMqCommEst m_pOnMqCommEst;	  // Callback for communication establishment
		OnMqCommIntr m_pOnMqCommIntr; // Callback for communication interruption
		OnMqMsg m_pOnMqMsg;			  // Callback for message handling
		OnMqExit m_OnMqExit;		  // Callback for message queue exit
	};
	typedef struct tagMqMsgCb MqMsgCb;

	// Define a structure for initializing message queue SDK parameters
	struct tagInitMqSdkParam
	{
		MqMsgCb* m_pMqMsgCb;			   // Pointer to message callback functions
		const char* m_pszWorkRoot;		   // Path to the working directory
		const unsigned int* m_pTopicArray; // List of Topics (array of strings)
		unsigned int m_nTopicCount;		   // Number of Topics
		unsigned int m_nClientID;		   // Client ID
	};
	typedef struct tagInitMqSdkParam SInitMqSdkParam;

#ifdef __cplusplus
}
#endif

#endif
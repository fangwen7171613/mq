#ifndef _Included_mq_MqClass
#define _Included_mq_MqClass

/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class mq_MqClass */

#ifdef CPP_STANDARD
extern "C"
{
#endif
	/*
	 * Class:     mq_MqClass
	 * Method:    Init
	 * Signature: (Ljava/lang/String;I[I)I
	 */
	JNIEXPORT jint JNICALL Java_mq_MqClass_init(JNIEnv* env, jobject obj, jstring strWorkRoot, jint nClientID, jintArray aryTopicID);

	/*
	 * Class:     mq_MqClass
	 * Method:    Exit
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_mq_MqClass_exit(JNIEnv* env, jobject obj);

	/*
	 * Class:     mq_MqClass
	 * Method:    PubTopicPollMqMsg
	 * Signature: (ILjava/lang/String;I)I
	 */
	JNIEXPORT jint JNICALL Java_mq_MqClass_publishTopicPollMessage(JNIEnv* env, jobject obj, jint nTopicID, jstring message, jint nMsgSize);

	/*
	 * Class:     mq_MqClass
	 * Method:    PubTopicBroadcastMqMsg
	 * Signature: (ILjava/lang/String;I)I
	 */
	JNIEXPORT jint JNICALL Java_mq_MqClass_publishTopicBroadcastMessage(JNIEnv* env, jobject obj, jint nTopicID, jstring message, jint nMsgSize);

	/*
	 * Class:     mq_MqClass
	 * Method:    PubAddrMqMsg
	 * Signature: (IILjava/lang/String;I)I
	 */
	JNIEXPORT jint JNICALL Java_mq_MqClass_publishAddressedMessage(JNIEnv* env, jobject obj, jint nClientID, jint nID, jint nTopicID, jstring message, jint nMsgSize);

#ifdef CPP_STANDARD
}
#endif
#endif

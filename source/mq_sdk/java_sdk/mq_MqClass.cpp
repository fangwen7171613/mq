#include "sdp/pch.h"
#include "sdp/log.h"
#include "sdp/comm_def.h"
#include "java_sdk/mq_MqClass.h"
#include "mq_c_sdk/mcs_interface.h"

JavaVM* g_JavaVM = nullptr;
MqMsgCb g_mqMsgCallback;

void OnJniReady();
void OnJniNotReady();
void OnJniExit();
void OnJniMqMsg(SDirectMsg* pMqMsg);

jint JNI_OnLoad(JavaVM* vm, [[maybe_unused]] void* reserved)
{
	g_JavaVM = vm;

	return JNI_VERSION_1_6;
}

jint JNICALL Java_mq_MqClass_init(JNIEnv* env, [[maybe_unused]] jobject obj, jstring strWorkRoot, jint nClientID, jintArray aryTopicID)
{
	// Get integer array elements
	jint* jniArray = env->GetIntArrayElements(aryTopicID, nullptr);
	if (!jniArray)
	{
		SDP_STARTUP_TRACE("topic array empty");

		return -1; // Early return on failure
	}

	jsize length = env->GetArrayLength(aryTopicID);

	std::vector<unsigned int> tempVector(static_cast<std::vector<unsigned int>::size_type>(length)); // Use vector for automatic memory management

	// Copy and convert each element
	std::transform(jniArray, jniArray + length, tempVector.begin(), [](jint elem)
				   { return static_cast<unsigned int>(elem); });
	// Release the JNI integer array elements
	env->ReleaseIntArrayElements(aryTopicID, jniArray, JNI_ABORT);

	// Get the work root string chars
	const char* workRootChars = env->GetStringUTFChars(strWorkRoot, nullptr);
	if (!workRootChars)
	{
		SDP_STARTUP_TRACE("Failed to get work root string chars");

		return -1; // Early return on failure
	}

	// Initialize the MQ client SDK parameter structure
#if CPP_STANDARD >= 20
	SInitMqSdkParam initParam = {
		.m_pMqMsgCb = &g_mqMsgCallback,
		.m_pszWorkRoot = workRootChars,
		.m_pTopicArray = tempVector.data(), // Vector's internal array
		.m_nTopicCount = static_cast<unsigned int>(length),
		.m_nClientID = static_cast<unsigned int>(nClientID)};
#else
	SInitMqSdkParam initParam = {
		&g_mqMsgCallback,
		workRootChars,
		tempVector.data(), // Vector's internal array
		static_cast<unsigned int>(length),
		static_cast<unsigned int>(nClientID)
	};
#endif

	// Register callbacks
	g_mqMsgCallback.m_pOnMqCommEst = OnJniReady;
	g_mqMsgCallback.m_pOnMqCommIntr = OnJniNotReady;
	g_mqMsgCallback.m_pOnMqMsg = OnJniMqMsg;

	// Initialize MQ SDK
	jint nRet = InitMqSdk(&initParam);

	// Release the work root string chars
	env->ReleaseStringUTFChars(strWorkRoot, workRootChars);

	return nRet; // Return the result of the initialization
}

void JNICALL Java_mq_MqClass_exit([[maybe_unused]] JNIEnv* env, [[maybe_unused]] jobject obj)
{
	ExitMqSdk();
}

jint JNICALL Java_mq_MqClass_publishTopicPollMessage(JNIEnv* env, [[maybe_unused]] jobject obj, jint nTopicID, jstring message, jint nMsgSize)
{
	SDP_MESSAGE_TRACE("Java_mq_MqClass_PubTopicPollMqMsg.");

	const char* msgStr = env->GetStringUTFChars(message, nullptr);
	if (msgStr)
		SDP_LIKELY20
		{
#if CPP_STANDARD >= 20
			STopicMsg topicMqMsg = {.m_nTopicID = static_cast<unsigned int>(nTopicID), .m_nMsgSize = static_cast<unsigned int>(nMsgSize), .m_pMsg = msgStr};
#else
			STopicMsg topicMqMsg = {static_cast<unsigned int>(nTopicID), static_cast<unsigned int>(nMsgSize), msgStr};
#endif

			jint nRet = ::PubTopRebMqMsg(&topicMqMsg);

			env->ReleaseStringUTFChars(message, msgStr);

			return nRet;
		}
	else
		SDP_UNLIKELY20
		{
			SDP_RUN_LOG_ERROR("get messge body failed");

			return -1;
		}
}

jint JNICALL Java_mq_MqClass_publishTopicBroadcastMessage(JNIEnv* env, [[maybe_unused]] jobject obj, jint nTopicID, jstring message, jint nMsgSize)
{
	const char* msgStr = env->GetStringUTFChars(message, nullptr);
	if (msgStr)
		SDP_LIKELY20
		{
#if CPP_STANDARD >= 20
			STopicMsg topicMqMsg = {.m_nTopicID = static_cast<unsigned int>(nTopicID), .m_nMsgSize = static_cast<unsigned int>(nMsgSize), .m_pMsg = msgStr};
#else
			STopicMsg topicMqMsg = {static_cast<unsigned int>(nTopicID), static_cast<unsigned int>(nMsgSize), msgStr};
#endif

			jint nRet = ::PubTopBcstMqMsg(&topicMqMsg);

			env->ReleaseStringUTFChars(message, msgStr);

			return nRet;
		}
	else
		SDP_UNLIKELY20
		{
			SDP_RUN_LOG_ERROR("get messge body failed");

			return -1;
		}
}

jint JNICALL Java_mq_MqClass_publishAddressedMessage(JNIEnv* env, [[maybe_unused]] jobject obj, jint nClientID, jint nID, jint nTopicID, jstring message, jint nMsgSize)
{
	const char* msgStr = env->GetStringUTFChars(message, nullptr);
	if (msgStr)
		SDP_LIKELY20
		{
#if CPP_STANDARD >= 20
			SDirectMsg addrMqMsg = {.m_nCorrID = static_cast<unsigned short>(nID), .m_nClientID = static_cast<unsigned int>(nClientID), .m_nTopicID = static_cast<unsigned int>(nTopicID), .m_nMsgSize = static_cast<unsigned int>(nMsgSize), .m_pMsg = const_cast<char*>(msgStr)};
#else
			SDirectMsg addrMqMsg = {static_cast<unsigned short>(nID), static_cast<unsigned int>(nClientID), static_cast<unsigned int>(nTopicID), static_cast<unsigned int>(nMsgSize), const_cast<char*>(msgStr)};
#endif

			jint nRet = ::PubDirMqMsg(&addrMqMsg);

			env->ReleaseStringUTFChars(message, msgStr);

			return nRet;
		}
	else
		SDP_UNLIKELY20
		{
			SDP_RUN_LOG_ERROR("get messge body failed");

			return -1;
		}
}

int GetJNIEnv(JNIEnv** env)
{
	*env = nullptr;

	jint status = g_JavaVM->GetEnv(reinterpret_cast<void**>(env), JNI_VERSION_1_6);
	if (status == JNI_EDETACHED)
	{
		if (g_JavaVM->AttachCurrentThread(reinterpret_cast<void**>(env), nullptr) == JNI_OK)
		{
			return 1; // Thread was detached and is now attached
		}
		else
		{
			return -1; // Failed to attach thread
		}
	}
	else if (status == JNI_OK)
	{
		return 0; // Thread was already attached
	}
	else
	{
		return -1; // Failed to get environment
	}
}

void DetachCurrentThreadIfNeeded(int attached)
{
	if (attached)
	{
		g_JavaVM->DetachCurrentThread();
	}
}

void CallJavaMethod(const char* className, const char* methodName, const char* methodSignature, ...)
{
	JNIEnv* env = nullptr;
	int attached = GetJNIEnv(&env);
	if (attached == -1)
	{
		SDP_RUN_LOG_ERROR("Failed to get JNI Environment");
		return;
	}

	jclass javaClass = env->FindClass(className);
	if (javaClass == nullptr)
	{
		SDP_RUN_LOG_ERROR("Failed to find class:{}", className);
		if (env->ExceptionCheck())
		{
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
		DetachCurrentThreadIfNeeded(attached);
		return;
	}

	jmethodID methodId = env->GetStaticMethodID(javaClass, methodName, methodSignature);
	if (methodId == nullptr)
	{
		SDP_RUN_LOG_ERROR("Failed to find method:{}, with signature:{}", methodName, methodSignature);
		if (env->ExceptionCheck())
		{
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
		env->DeleteLocalRef(javaClass);
		DetachCurrentThreadIfNeeded(attached);
		return;
	}

	va_list args;
	va_start(args, methodSignature);
	env->CallStaticVoidMethodV(javaClass, methodId, args);
	if (env->ExceptionCheck())
	{
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
	va_end(args);

	env->DeleteLocalRef(javaClass);
	DetachCurrentThreadIfNeeded(attached);
}

// version1，可以稳定运行，就是效率低
//  void OnJniReady()
//{
//	CallJavaMethod("mq/MqClass", "onReady", "()V");
//  }
//
//  void OnJniNotReady()
//{
//	CallJavaMethod("mq/MqClass", "onNotReady", "()V");
//  }
//
//  void OnJniExit()
//{
//	CallJavaMethod("mq/MqClass", "onExit", "()V");
//  }
//
//  void OnJniMqMsg(SDirectMsg* pMqMsg)
//{
//	SDP_MESSAGE_TRACE("OnJniMqMsg.");
//
//	JNIEnv* env = nullptr;
//	int attached = GetJNIEnv(&env);
//	if (attached == -1)
//		return;
//
//	jclass javaClass = env->FindClass("mq/MqClass");
//	if (javaClass == nullptr)
//	{
//		DetachCurrentThreadIfNeeded(attached);
//		return;
//	}
//
//	jmethodID methodId = env->GetStaticMethodID(javaClass, "onPublishAddressedMessage", "(IILjava/lang/String;I)V");
//	if (methodId == nullptr)
//	{
//		env->DeleteLocalRef(javaClass);
//		DetachCurrentThreadIfNeeded(attached);
//		return;
//	}
//
//	jstring jMessage = env->NewStringUTF(pMqMsg->m_pMsg);
//	env->CallStaticVoidMethod(javaClass, methodId, pMqMsg->m_nClientID, pMqMsg->m_nTopicID, jMessage, pMqMsg->m_nMsgSize);
//
//	env->DeleteLocalRef(jMessage);
//	env->DeleteLocalRef(javaClass);
//	DetachCurrentThreadIfNeeded(attached);
//  }

// 假设 somewhere else it's being assigned
namespace
{
	JNIEnv* g_env = nullptr;
	jclass g_javaClass = nullptr;
	jmethodID g_methodId = nullptr;
} // namespace

bool EnsureJNIEnv()
{
	if (g_env != nullptr)
	{
		return true;
	}

	JNIEnv* local_env = nullptr;
	void* temp_env = nullptr;

	if (g_JavaVM->GetEnv(&temp_env, JNI_VERSION_1_6) != JNI_OK)
	{
		if (g_JavaVM->AttachCurrentThread(&temp_env, nullptr) != JNI_OK)
		{
			SDP_RUN_LOG_ERROR("Failed to attach current thread to JVM");
			return false;
		}
	}

	local_env = static_cast<JNIEnv*>(temp_env);
	g_env = local_env;
	return true;
}

void DetachCurrentThreadIfNeeded()
{
	if (g_env)
	{
		g_JavaVM->DetachCurrentThread();
		g_env = nullptr; // Reset attachment flag
	}
}

void CacheClassAndMethod()
{
	if (g_javaClass == nullptr)
	{
		jclass localClass = g_env->FindClass("mq/MqClass");
		if (localClass == nullptr)
		{
			SDP_RUN_LOG_ERROR("Failed to find class: mq/MqClass");
			if (g_env->ExceptionCheck())
			{
				g_env->ExceptionDescribe();
				g_env->ExceptionClear();
			}
			return;
		}
		// 转换为全局引用
		g_javaClass = static_cast<jclass>(g_env->NewGlobalRef(localClass));
		g_env->DeleteLocalRef(localClass); // 删除局部引用
	}

	if (g_methodId == nullptr)
	{
		g_methodId = g_env->GetStaticMethodID(g_javaClass, "onPublishAddressedMessage", "(IIILjava/lang/String;I)V");
		if (g_methodId == nullptr)
		{
			SDP_RUN_LOG_ERROR("Method onPublishAddressedMessage not found");
			if (g_env->ExceptionCheck())
			{
				g_env->ExceptionDescribe();
				g_env->ExceptionClear();
			}
			// 清理全局引用
			g_env->DeleteGlobalRef(g_javaClass);
			g_javaClass = nullptr;
			return;
		}
	}
}

void ReleaseCachedResources(JNIEnv* env)
{
	if (g_javaClass != nullptr)
	{
		env->DeleteGlobalRef(g_javaClass); // 使用DeleteGlobalRef释放全局引用
		g_javaClass = nullptr;
	}
	g_methodId = nullptr; // g_methodId是一个ID，不需要释放，但应重置指示器
}

void OnJniReady()
{
	if (EnsureJNIEnv())
	{
		CacheClassAndMethod();
	}

	CallJavaMethod("mq/MqClass", "onReady", "()V");
}

void OnJniNotReady()
{
	CallJavaMethod("mq/MqClass", "onNotReady", "()V");
}

void OnJniExit()
{
	ReleaseCachedResources(g_env);
	DetachCurrentThreadIfNeeded();
	CallJavaMethod("mq/MqClass", "onExit", "()V");
}

void OnJniMqMsg(SDirectMsg* pMqMsg)
{
	if (g_env && g_javaClass && g_methodId)
	{ // 确保环境和全局引用有效
		jstring jMessage = g_env->NewStringUTF(pMqMsg->m_pMsg);
		if (jMessage)
			SDP_LIKELY20
			{
				g_env->CallStaticVoidMethod(g_javaClass, g_methodId, pMqMsg->m_nClientID, pMqMsg->m_nCorrID, pMqMsg->m_nTopicID, jMessage, pMqMsg->m_nMsgSize);

				if (g_env->ExceptionCheck())
				{ // 检查调用Java方法后是否有异常发生
					g_env->ExceptionDescribe();
					g_env->ExceptionClear();
				}

				g_env->DeleteLocalRef(jMessage); // 清理局部引用
			}
		else
			SDP_UNLIKELY20
			{ // 检查NewStringUTF是否成功
				// 处理错误情况，比如记录日志
				SDP_RUN_LOG_ERROR("Failed to create jstring from message.");
				if (g_env->ExceptionCheck())
				{
					g_env->ExceptionDescribe();
					g_env->ExceptionClear();
				}
				return;
			}
	}
	else
	{
		// 如果g_env, g_javaClass或g_methodId无效，可以在这里记录日志或处理错误
		SDP_RUN_LOG_ERROR("JNI environment or class/method ID not properly initialized.");
	}
}
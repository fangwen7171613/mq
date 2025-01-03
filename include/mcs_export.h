#ifndef MQ_CLIENT_SDK_EXPORT_H
#define MQ_CLIENT_SDK_EXPORT_H

#ifdef _WIN32
#ifdef DECLARE_MQ_C_EXPORT
#define MQ_C_EXPORT __declspec(dllexport)
#else
#define MQ_C_EXPORT __declspec(dllimport)
#endif
#elif defined(__GNUC__) || defined(__clang__)
#ifdef DECLARE_MQ_C_EXPORT
#define MQ_C_EXPORT __attribute__((visibility("default")))
#else
#define MQ_C_EXPORT
#endif
#else
#define MQ_C_EXPORT
#endif

#endif
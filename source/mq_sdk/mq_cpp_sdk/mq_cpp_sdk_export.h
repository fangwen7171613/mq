#pragma once

#ifdef _WIN32
#ifdef DECLARE_MQ_CPP_EXPORT
#define MQ_CPP_EXPORT __declspec(dllexport)
#else
#define MQ_CPP_EXPORT __declspec(dllimport)
#endif
#elif defined(__GNUC__) || defined(__clang__)
#ifdef DECLARE_MQ_CPP_EXPORT
#define MQ_CPP_EXPORT __attribute__((visibility("default")))
#else
#define MQ_CPP_EXPORT
#endif
#else
#define MQ_CPP_EXPORT
#endif
#pragma once
#include "Debug.hpp"

namespace DXR
{
	void LogError(wchar_t* file, unsigned int line, wchar_t* message);
	void LogWarning(wchar_t* file, unsigned int line, wchar_t* message);
	void LogSuccess(wchar_t* file, unsigned int line, wchar_t* message);
	void LogInfo(wchar_t* file, unsigned int line, wchar_t* message);

#define ERROR_LOG(Message) LogError(__FILEW__,__LINE__,Message)
#define WARNING_LOG(Message) LogWarning(__FILEW__,__LINE__,Message)
#define SUCCESS_LOG(Message) LogWarning(__FILEW__,__LINE__,Message)
#define INFO_LOG(Message) LogWarning(__FILEW__,__LINE__,Message)
#define LOG(Message) wprintf_s(Message)
}

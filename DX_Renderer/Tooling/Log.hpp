#pragma once
#include "Debug.hpp"

namespace DXR
{
	void LogError(wchar_t* file, unsigned int line, wchar_t* message);
	void LogWarning(wchar_t* file, unsigned int line, wchar_t* message);
	void LogSuccess(wchar_t* file, unsigned int line, wchar_t* message);
	void LogInfo(wchar_t* file, unsigned int line, wchar_t* message);

#define __FILENAME__ (wcsrchr(__FILEW__, '\\') ? wcsrchr(__FILEW__, '\\') + 1 : __FILEW__)

#define ERROR_LOG(Message) LogError(__FILENAME__,__LINE__,Message)
#define WARNING_LOG(Message) LogWarning(__FILENAME__,__LINE__,Message)
#define SUCCESS_LOG(Message) LogSuccess(__FILENAME__,__LINE__,Message)
#define INFO_LOG(Message) LogInfo(__FILENAME__,__LINE__,Message)
#define LOG(Message) wprintf_s(Message)
}

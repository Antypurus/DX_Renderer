#pragma once

namespace DXR
{
	void LogError(wchar_t* file, unsigned int line, wchar_t* message);
	void LogError(wchar_t* file, unsigned int line, char* message);
	void LogWarning(wchar_t* file, unsigned int line, wchar_t* message);
	void LogWarning(wchar_t* file, unsigned int line, char* message);
	void LogSuccess(wchar_t* file, unsigned int line, wchar_t* message);
	void LogSuccess(wchar_t* file, unsigned int line, char* message);
	void LogInfo(wchar_t* file, unsigned int line, wchar_t* message);
	void LogInfo(wchar_t* file, unsigned int line, char* message);

#define __FILENAME__ (wcsrchr(__FILEW__, '\\') ? wcsrchr(__FILEW__, '\\') + 1 : __FILEW__)

#ifndef NDEBUG
#define ERROR_LOG(Message) DXR::LogError(__FILENAME__,__LINE__,Message)
#define WARNING_LOG(Message) DXR::LogWarning(__FILENAME__,__LINE__,Message)
#define SUCCESS_LOG(Message) DXR::LogSuccess(__FILENAME__,__LINE__,Message)
#define INFO_LOG(Message) DXR::LogInfo(__FILENAME__,__LINE__,Message)
#define LOG(Message) wprintf_s(Message)
#else
#define ERROR_LOG(Message)
#define WARNING_LOG(Message)
#define SUCCESS_LOG(Message)
#define INFO_LOG(Message)
#define LOG(Message)
#endif

}

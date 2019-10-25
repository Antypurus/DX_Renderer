#include "Debug.hpp"
#include <string>

#define internal static

void LogError(wchar_t* file, unsigned int line, wchar_t* message)
{
	wprintf_s(L"[ERROR]@%s:%d::%s", file, line, message);
}

#define ERROR_LOG(Message) LogError(__FILEW__,__LINE__,Message)
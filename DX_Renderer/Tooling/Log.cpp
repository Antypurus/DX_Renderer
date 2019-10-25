#include "Log.hpp"

namespace DXR
{
	void LogError(wchar_t* file, unsigned int line, wchar_t* message)
	{
		wprintf_s(L"[ERROR]@%s:%d::%s", file, line, message);
	}

	void LogWarning(wchar_t* file, unsigned int line, wchar_t* message)
	{
		wprintf_s(L"[WARNING]@%s:%d::%s", file, line, message);
	}

	void LogSuccess(wchar_t* file, unsigned int line, wchar_t* message)
	{
		wprintf_s(L"[SUCCESS]@%s:%d::%s", file, line, message);
	}

	void LogInfo(wchar_t* file, unsigned int line, wchar_t* message)
	{
		wprintf_s(L"[INFO]@%s:%d::%s", file, line, message);
	}
}

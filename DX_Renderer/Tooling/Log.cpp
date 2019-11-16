#include "Log.hpp"
#include <string>
#include <wrl.h>
#include "Debug.hpp"

namespace DXR
{

	static HANDLE consoleHandle;
	
	void LogError(wchar_t* file, unsigned int line, wchar_t* message)
	{
		HANDLE hConsole = Debug::ConsoleHandle;
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		wprintf_s(L"[ERROR]@%s:%d=>%s", file, line, message);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED);
	}

	void LogWarning(wchar_t* file, unsigned int line, wchar_t* message)
	{
		HANDLE hConsole = Debug::ConsoleHandle;
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED|FOREGROUND_GREEN);
		wprintf_s(L"[WARNING]@%s:%d=>%s", file, line, message);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED);
	}

	void LogSuccess(wchar_t* file, unsigned int line, wchar_t* message)
	{
		HANDLE hConsole = Debug::ConsoleHandle;
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
		wprintf_s(L"[SUCCESS]@%s:%d=>%s", file, line, message);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED);
	}

	void LogInfo(wchar_t* file, unsigned int line, wchar_t* message)
	{
		HANDLE hConsole = Debug::ConsoleHandle;
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE|FOREGROUND_GREEN);
		wprintf_s(L"[INFO]@%s:%d=>%s", file, line, message);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED);
	}
}

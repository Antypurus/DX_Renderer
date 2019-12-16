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
		wprintf_s(L"[ERROR]@%ls:%d=>%ls\n", file, line, message);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}

	void LogError(wchar_t* file, unsigned line, char* message)
	{
		HANDLE hConsole = Debug::ConsoleHandle;
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		printf_s("[ERROR]@%ls:%d=>%s\n", file, line, message);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}

	void LogWarning(wchar_t* file, unsigned int line, wchar_t* message)
	{
		HANDLE hConsole = Debug::ConsoleHandle;
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
		wprintf_s(L"[WARNING]@%ls:%d=>%ls\n", file, line, message);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}

	void LogWarning(wchar_t* file, unsigned line, char* message)
	{
		HANDLE hConsole = Debug::ConsoleHandle;
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
		printf_s("[WARNING]@%ls:%d=>%s\n", file, line, message);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}

	void LogSuccess(wchar_t* file, unsigned int line, wchar_t* message)
	{
		HANDLE hConsole = Debug::ConsoleHandle;
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
		wprintf_s(L"[SUCCESS]@%ls:%d=>%ls\n", file, line, message);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}

	void LogSuccess(wchar_t* file, unsigned line, char* message)
	{
		HANDLE hConsole = Debug::ConsoleHandle;
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
		printf_s("[SUCCESS]@%ls:%d=>%s\n", file, line, message);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}

	void LogInfo(wchar_t* file, unsigned int line, wchar_t* message)
	{
		HANDLE hConsole = Debug::ConsoleHandle;
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
		wprintf_s(L"[INFO]@%ls:%d=>%ls\n", file, line, message);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}

	void LogInfo(wchar_t* file, unsigned line, char* message)
	{
		HANDLE hConsole = Debug::ConsoleHandle;
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
		printf_s("[INFO]@%lls:%d=>%s\n", file, line, message);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}
}

#pragma once

#include "Log.hpp"

wchar_t* FormatMessage(HRESULT res)
{
	LPWSTR messageBuffer = nullptr;
	size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, res, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);
	return messageBuffer;
}

#ifndef NDEBUG
#define DXCall(x)\
	HRESULT result_code = x;\
	if(result_code < 0)\
	{\
		wchar_t* error_message = FormatMessage(result_code);\
		ERROR_LOG(error_message); \
		free(error_message);\
		__debugbreak();\
	}
#else
#define DXCall(x) x;
#endif
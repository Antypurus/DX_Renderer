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
	HRESULT res = x;\
	if(res < 0)\
	{\
		ERROR_LOG(FormatMessage(res)); \
		__debugbreak();\
		exit(-1);\
	}
#else
#define DXCall(x) x;
#endif
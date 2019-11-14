#pragma once
#include <wrl.h>
#include "Log.hpp"

namespace DXR
{
	wchar_t* FormatMessage(HRESULT res);

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
}
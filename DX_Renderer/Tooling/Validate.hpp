#pragma once
#include <wrl.h>
#include "Log.hpp"

namespace DXR
{
	wchar_t* FormatErrorMessage(HRESULT res);

#define CAT_(x,y) x ## y
#define CAT(x,y) CAT_(x,y)

#ifndef NDEBUG
#define DXCall(x)\
	HRESULT CAT(result_code,__LINE__) = x;if(CAT(result_code,__LINE__) < 0){wchar_t* CAT(error_message,__LINE__) = FormatErrorMessage(CAT(result_code,__LINE__));DXR::LogError(__FILENAME__, __LINE__, CAT(error_message,__LINE__));free(CAT(error_message,__LINE__));__debugbreak();}
#else
#define DXCall(x) x;
#endif
}
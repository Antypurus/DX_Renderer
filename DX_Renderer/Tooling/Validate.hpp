#pragma once
#include <wrl.h>
#include "Log.hpp"

namespace DXR
{
	wchar_t* FormatMessage(HRESULT res);
	void ValidateDX12Call(HRESULT res,wchar_t* filename, size_t line);

#ifndef NDEBUG
#define DXCall(x)\
	ValidateDX12Call(x,__FILENAME__,__LINE__);
#else
#define DXCall(x) x;
#endif
}
#pragma once

#include "Debug.hpp"

#ifndef NDEBUG
#define DXCall(x)\
	HRESULT res = x;\
	if(res < 0)\
	{\
		DXR::Debug::DebugInterface.Log(__FILE__); \
			__debugbreak();\
	}
#else
#define DXCall(x) x;
#endif
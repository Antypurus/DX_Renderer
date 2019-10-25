#pragma once

#ifdef DEBUG
#define DXCall(x)\
	HRESULT res = x;\
	
#else
#define DXCall(x) x;
#endif
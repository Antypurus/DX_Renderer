#include "Validate.hpp"

namespace DXR
{
	wchar_t* FormatMessage(HRESULT res)
	{
		LPWSTR messageBuffer = nullptr;
		size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, res, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);
		return messageBuffer;
	}
}
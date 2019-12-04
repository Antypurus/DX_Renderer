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

	void ValidateDX12Call(HRESULT res, wchar_t* filename, size_t line)
	{
		HRESULT result_code = res;
		if(result_code < 0)
		{
			wchar_t* error_message = FormatMessage(result_code);
			DXR::LogError(filename,line,error_message);
			free(error_message);
			__debugbreak();
		}
	}
}

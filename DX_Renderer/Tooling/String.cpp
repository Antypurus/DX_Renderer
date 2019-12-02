#include "String.hpp"

//TODO: This can likely be optimized to reduce allocations
std::unique_ptr<wchar_t> String::ConvertChartToWideChar(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	size_t converted_chars = 0;
	mbstowcs_s(&converted_chars, wc, cSize, c, _TRUNCATE);

	std::unique_ptr<wchar_t> wide_string = std::make_unique<wchar_t>(*wc);
	delete[] wc;

	return wide_string;
}

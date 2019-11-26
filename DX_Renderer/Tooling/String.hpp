#pragma once
#include <memory>

class String
{
public:
	static std::unique_ptr<wchar_t> ConvertChartToWideChar(const char *c);
};


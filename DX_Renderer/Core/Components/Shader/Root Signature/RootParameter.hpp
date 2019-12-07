#pragma once

#include <d3d12.h>

namespace DXR
{

	enum class RootParameterType
	{
		RootConstant,
		RootDescriptor,
		DescriptorTable
	};

	struct RootParameter
	{
	public:
	protected:
	public:
	protected:
		RootParameter(RootParameterType rootParameterType);
	};

}
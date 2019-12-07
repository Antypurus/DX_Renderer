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
		RootParameterType m_root_parameter_type;
		D3D12_SHADER_VISIBILITY m_shader_visibility;
		D3D12_ROOT_PARAMETER_TYPE m_parameter_type;
	public:
	protected:
		RootParameter(RootParameterType rootParameterType, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
	};

}
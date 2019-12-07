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
		virtual ~RootParameter() = default;
	protected:
		RootParameterType m_root_parameter_type;
		D3D12_SHADER_VISIBILITY m_shader_visibility;
		D3D12_ROOT_PARAMETER_TYPE m_parameter_type;
	public:
		D3D12_ROOT_PARAMETER GetRootParameter();
	protected:
		RootParameter(RootParameterType rootParameterType, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		virtual D3D12_ROOT_PARAMETER GenerateRootParameter() = 0;
	};

}
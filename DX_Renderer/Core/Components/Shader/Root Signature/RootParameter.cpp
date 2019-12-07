#include "RootParameter.hpp"

namespace DXR
{
	D3D12_ROOT_PARAMETER RootParameter::GetRootParameter()
	{
		return this->GenerateRootParameter();
	}

	RootParameter::RootParameter(RootParameterType rootParameterType, D3D12_SHADER_VISIBILITY visibility)
		:m_root_parameter_type(rootParameterType), m_shader_visibility(visibility)
	{
		switch(rootParameterType)
		{
			case DXR::RootParameterType::RootConstant:
			{
				this->m_parameter_type = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
				break;
			}
			case DXR::RootParameterType::RootDescriptor:
			{
				// Needs to be set by the subclass for the root descriptor
				break;
			}
			case DXR::RootParameterType::DescriptorTable:
			{
				this->m_parameter_type = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				break;
			}
			default:
				break;
		}
	}
}

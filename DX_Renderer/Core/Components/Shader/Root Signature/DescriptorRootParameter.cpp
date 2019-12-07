#include "DescriptorRootParameter.hpp"

namespace DXR
{
	DescriptorRootParameter::DescriptorRootParameter(DescriptorType Type, UINT registerSlot,
													 D3D12_SHADER_VISIBILITY visibility)
		:RootParameter(RootParameterType::RootDescriptor, visibility), m_register_slot(registerSlot)
	{
		switch(Type)
		{
			case DXR::DescriptorType::CBV:
			{
				this->m_parameter_type = D3D12_ROOT_PARAMETER_TYPE_CBV;
				break;
			}
			case DXR::DescriptorType::UAV:
			{
				this->m_parameter_type = D3D12_ROOT_PARAMETER_TYPE_UAV;
				break;
			}
			case DXR::DescriptorType::SRV:
			{
				this->m_parameter_type = D3D12_ROOT_PARAMETER_TYPE_SRV;
				break;
			}
			default:
				break;
		}
	}

	D3D12_ROOT_PARAMETER DescriptorRootParameter::GenerateRootParameter()
	{
		D3D12_ROOT_DESCRIPTOR root_descriptor = {};
		root_descriptor.RegisterSpace = 0;
		root_descriptor.ShaderRegister = this->m_register_slot;

		D3D12_ROOT_PARAMETER root_parameter = {};
		root_parameter.Descriptor = root_descriptor;
		root_parameter.ParameterType = this->m_parameter_type;
		root_parameter.ShaderVisibility = this->m_shader_visibility;

		return root_parameter;
	}
}

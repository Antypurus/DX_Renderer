#include "ConstantRootParameter.hpp"

namespace DXR
{
	ConstantRootParameter::ConstantRootParameter(UINT registerSlot, UINT numberOfValues,
		D3D12_SHADER_VISIBILITY visibility):
	RootParameter(RootParameterType::RootConstant,visibility),m_register_slot(registerSlot),m_number_of_values(numberOfValues)
	{
	}

	D3D12_ROOT_PARAMETER ConstantRootParameter::GenerateRootParameter()
	{
		D3D12_ROOT_CONSTANTS root_constants = {};
		root_constants.Num32BitValues = this->m_number_of_values;
		root_constants.RegisterSpace = 0;
		root_constants.ShaderRegister = this->m_register_slot;

		D3D12_ROOT_PARAMETER root_parameter = {};
		root_parameter.Constants = root_constants;
		root_parameter.ParameterType = this->m_parameter_type;
		root_parameter.ShaderVisibility = this->m_shader_visibility;

		return root_parameter;
	}
}

#include "RootSignature.hpp"
#include "../../GraphicsDevice.hpp"
#include "../../../../Tooling/Validate.hpp"

namespace DXR
{
	RootSignature::RootSignature()
	{
	}

	void RootSignature::AddConstantRootParameter(ConstantRootParameter& rootParameter)
	{
		this->m_constant_root_parameters.push_back(rootParameter);
	}

	void RootSignature::AddDescriptorRootParameter(DescriptorRootParameter& rootParameter)
	{
		this->m_descriptor_root_parameters.push_back(rootParameter);
	}

	void RootSignature::AddDescriptorTableRootParameter(DescriptorTableRootParameter& rootParameter)
	{
		this->m_descriptor_table_root_parameters.push_back(rootParameter);
	}

	void RootSignature::CreateRootSignature(GraphicsDevice& device)
	{
		UINT root_parameter_count = this->m_constant_root_parameters.size() + this->m_descriptor_root_parameters.size()
			+ this->m_descriptor_table_root_parameters.size();
		D3D12_ROOT_PARAMETER* root_parameters = new D3D12_ROOT_PARAMETER[root_parameter_count];

		size_t offset = 0;
		for(size_t i = 0;i < this->m_constant_root_parameters.size();++i)
		{
			root_parameters[offset + i] = this->m_constant_root_parameters[i].GetRootParameter();
		}

		offset = this->m_constant_root_parameters.size();
		for(size_t i = 0;i < this->m_descriptor_root_parameters.size();++i)
		{
			root_parameters[offset + i] = this->m_descriptor_root_parameters[i].GetRootParameter();
		}

		offset = this->m_descriptor_root_parameters.size();
		for(size_t i = 0;i < this->m_descriptor_table_root_parameters.size();++i)
		{
			root_parameters[offset + i] = this->m_descriptor_table_root_parameters[i].GetRootParameter();
		}

		D3D12_ROOT_SIGNATURE_DESC root_signature = {};
		root_signature.NumParameters = root_parameter_count;
		root_signature.pParameters = root_parameters;
		root_signature.NumStaticSamplers = 0;
		root_signature.pStaticSamplers = nullptr;
		root_signature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		WRL::ComPtr<ID3DBlob> serialized_root_signature;
		WRL::ComPtr<ID3DBlob> errors;
		DXCall(D3D12SerializeRootSignature(&root_signature, D3D_ROOT_SIGNATURE_VERSION_1, &serialized_root_signature, &errors));

		if(errors != nullptr)
		{
			ERROR_LOG((char*)errors->GetBufferPointer());
			errors->Release();
			MessageBox(NULL, "Failed To Compile Shader", "Error", MB_ICONEXCLAMATION | MB_OK);
			throw std::exception("Failed To Compile Shader");
		}

		DXCall(device->CreateRootSignature(0, serialized_root_signature->GetBufferPointer(),
										   serialized_root_signature->GetBufferSize(),
										   IID_PPV_ARGS(&this->m_root_signature)));

		delete[] root_parameters;
	}

	ID3D12RootSignature* RootSignature::GetRootSignature() const
	{
		return this->m_root_signature.Get();
	}
}

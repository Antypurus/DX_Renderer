#include "IntersectionShader.hpp"

namespace DXR
{
	IntersectionShader IntersectionShader::CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint)
	{
		IntersectionShader shader(filename, entryPoint);
		shader.CompileFromFile(filename, entryPoint);
		return shader;
	}

	D3D12_STATE_SUBOBJECT IntersectionShader::GenerateShaderExport()
	{
		shader_export_desc.Name = this->UniqueID.c_str();
		shader_export_desc.ExportToRename = this->entryPoint.c_str();
		shader_export_desc.Flags = D3D12_EXPORT_FLAG_NONE;

		library_desc.DXILLibrary.BytecodeLength = this->m_shader_code->GetBufferSize();
		library_desc.DXILLibrary.pShaderBytecode = this->m_shader_code->GetBufferPointer();
		library_desc.NumExports = 1;
		library_desc.pExports = &shader_export_desc;

		shader.pDesc = &library_desc;
		shader.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;

		return shader;
	}

	std::wstring& IntersectionShader::GetUniqueID()
	{
		return this->UniqueID;
	}

	IntersectionShader::IntersectionShader(const std::wstring& filepath, const std::wstring& entryPoint) :Shader(ShaderType::RayTracingShader)
	{
		this->filepath = filepath;
		this->entryPoint = entryPoint;
		this->UniqueID = filepath + entryPoint;
	}
}
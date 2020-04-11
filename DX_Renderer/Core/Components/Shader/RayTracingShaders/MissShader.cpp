#include "MissShader.hpp"

namespace DXR
{
	MissShader MissShader::CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint)
	{
		MissShader shader(filename, entryPoint);
		shader.CompileFromFile(filename, entryPoint);
		return shader;
	}

	D3D12_STATE_SUBOBJECT MissShader::GenerateShaderExport()
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

	std::wstring& MissShader::GetUniqueID()
	{
		return this->UniqueID;
	}

	MissShader::MissShader(const std::wstring& filepath, const std::wstring& entryPoint) :Shader(ShaderType::RayTracingShader)
	{
		this->filepath = filepath;
		this->entryPoint = entryPoint;
		this->UniqueID = filepath + entryPoint;
	}
}
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
		D3D12_EXPORT_DESC shader_export = {};
		shader_export.Name = this->UniqueID.c_str();
		shader_export.ExportToRename = this->entryPoint.c_str();
		shader_export.Flags = D3D12_EXPORT_FLAG_NONE;

		D3D12_DXIL_LIBRARY_DESC lib_desc = {};
		lib_desc.DXILLibrary.BytecodeLength = this->m_shader_code->GetBufferSize();
		lib_desc.DXILLibrary.pShaderBytecode = this->m_shader_code->GetBufferPointer();
		lib_desc.NumExports = 1;
		lib_desc.pExports = &shader_export;

		D3D12_STATE_SUBOBJECT ms = {};
		ms.pDesc = &lib_desc;
		ms.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;

		return ms;
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
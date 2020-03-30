#include "ClosestHitShader.hpp"

namespace DXR
{
	ClosestHitShader ClosestHitShader::CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint)
	{
		ClosestHitShader shader(filename,entryPoint);
		shader.CompileFromFile(filename,entryPoint);
		return shader;
	}

	D3D12_STATE_SUBOBJECT ClosestHitShader::GenerateShaderExport()
	{
		D3D12_EXPORT_DESC shader_export = {};
		shader_export.Name = this->UniqueID.c_str();
		shader_export.ExportToRename = L"ClosestHitShader";
		shader_export.Flags = D3D12_EXPORT_FLAG_NONE;

		D3D12_DXIL_LIBRARY_DESC lib_desc = {};
		lib_desc.DXILLibrary.BytecodeLength = this->m_shader_code->GetBufferSize();
		lib_desc.DXILLibrary.pShaderBytecode = this->m_shader_code->GetBufferPointer();
		lib_desc.NumExports = 1;
		lib_desc.pExports = &shader_export;

		D3D12_STATE_SUBOBJECT chs = {};
		chs.pDesc = &lib_desc;
		chs.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;

		return chs;
	}

	ClosestHitShader::ClosestHitShader(const std::wstring& filepath, const std::wstring& entryPoint):Shader(ShaderType::RayTracingShader)
	{
		this->filepath = filepath;
		this->entryPoint = entryPoint;
		this->UniqueID = filepath + entryPoint;
	}
}
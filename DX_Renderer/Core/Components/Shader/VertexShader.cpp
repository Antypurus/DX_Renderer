#include "VertexShader.hpp"
#include <d3dcompiler.h>
#include "../../../Tooling/Log.hpp"
#include "../../../Tooling/String.hpp"

namespace DXR
{
	VertexShader::VertexShader() :Shader(ShaderType::VertexShader) {}

	VertexShader VertexShader::CompileShaderFromFile(const std::wstring& filename, const std::string& entryPoint)
	{
		VertexShader shader;
		shader.CompileFromFile(filename, entryPoint);
		return shader;
	}

	VertexShader VertexShader::CompileShader(const std::string& shaderCode, const std::string& entryPoint)
	{
		VertexShader shader;
		shader.Compile(shaderCode, entryPoint);
		return shader;
	}

	void VertexShader::CompileFromFile(const std::wstring& filename, const std::string& entryPoint)
	{
		ID3DBlob* shader_code;
		ID3DBlob* error_msg;
		UINT compilation_flags = 0;

#ifndef NDEBUG
		compilation_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		D3DCompileFromFile(filename.c_str(), nullptr, nullptr, entryPoint.c_str(), SHADER_TYPE_NAME, compilation_flags, 0, &shader_code, &error_msg);

		// validate compilation errors
		if(error_msg != nullptr)
		{
			ERROR_LOG(String::ConvertChartToWideChar((char*)error_msg->GetBufferPointer()).get());
			error_msg->Release();
			MessageBox(NULL, "Failed To Compile Vertex Shader", "Error", MB_ICONEXCLAMATION | MB_OK);
			throw std::exception("Failed To Compile Vertex Shader");
		}

		this->m_shader_code.Reset();
		this->m_shader_code = WRL::ComPtr<ID3DBlob>(shader_code);
	}

	void VertexShader::Compile(const std::string& shaderCode, const std::string& entryPoint)
	{
		ID3DBlob* shader_code;
		ID3DBlob* error_msg;
		UINT compilation_flags = 0;

#ifndef NDEBUG
		compilation_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		D3DCompile(shaderCode.c_str(), shaderCode.length() + 1, nullptr, nullptr, nullptr, entryPoint.c_str(), SHADER_TYPE_NAME, compilation_flags, 0, &shader_code, &error_msg);

		// validate compilation errors
		if(error_msg != nullptr)
		{
			ERROR_LOG(String::ConvertChartToWideChar((char*)error_msg->GetBufferPointer()).get());
			error_msg->Release();
			MessageBox(NULL, "Failed To Compile Vertex Shader", "Error", MB_ICONEXCLAMATION | MB_OK);
			throw std::exception("Failed To Compile Vertex Shader");
		}

		this->m_shader_code.Reset();
		this->m_shader_code = WRL::ComPtr<ID3DBlob>(shader_code);
	}
}

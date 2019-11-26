#include "VertexShader.hpp"
#include <d3dcompiler.h>
#include "../../../Tooling/Log.hpp"
#include "../../../Tooling/String.hpp"

namespace DXR
{
	VertexShader::VertexShader() :Shader(ShaderType::VertexShader) {}

	VertexShader VertexShader::CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint)
	{
	}

	VertexShader VertexShader::CompileShadert(const std::wstring& shaderCode, const std::wstring& entryPoint)
	{
	}

	ID3DBlob* VertexShader::CompileFromFile(const std::wstring& filename, const std::string& entryPoint)
	{
		ID3DBlob* shader_code;
		ID3DBlob* error_msg;
		UINT compilation_flags = 0;
		
#ifndef NDEBUG
		compilation_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		
		D3DCompileFromFile(filename.c_str(), nullptr, nullptr, entryPoint.c_str(), "vs_5_1", compilation_flags, 0, &shader_code, &error_msg);

		// validate compilation errors
		if(error_msg!=nullptr)
		{
			ERROR_LOG(String::ConvertChartToWideChar((char*)error_msg->GetBufferPointer()).get());
			error_msg->Release();
			MessageBox(NULL,"Failed To Compile Vertex Shader","Error",MB_ICONEXCLAMATION | MB_OK);
			throw std::exception("Failed To Compile Vertex Shader");
		}

		return shader_code;
	}

	ID3DBlob* VertexShader::Compile(const std::string& shaderCode, const std::string& entryPoint)
	{
		return nullptr;
	}
}

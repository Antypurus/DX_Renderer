#include "PixelShader.hpp"
#include "../../../Tooling/Log.hpp"

namespace DXR
{
	PixelShader PixelShader::CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint)
	{
		PixelShader shader;
		INFO_LOG(L"Started Compiling Pixel Shader");
		shader.CompileFromFile(filename, entryPoint);
		SUCCESS_LOG(L"Pixel Shader Compiled");
		return shader;
	}

	PixelShader PixelShader::CompileShader(const std::wstring& shaderCode, const std::wstring& entryPoint)
	{
		PixelShader shader;
		INFO_LOG(L"Started Compiling Pixel Shader");
		shader.Compile(shaderCode, entryPoint);
		SUCCESS_LOG(L"Pixel Shader Compiled");
		return shader;
	}

	PixelShader::PixelShader() : Shader(ShaderType::PixelShader)
	{
	}

}

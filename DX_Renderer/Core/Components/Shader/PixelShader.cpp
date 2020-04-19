#include "PixelShader.hpp"
#include "../../../Tooling/Log.hpp"

namespace DXR
{
	PixelShader PixelShader::CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint)
	{
		PixelShader shader(filename,entryPoint);
		INFO_LOG(L"Started Compiling Pixel Shader");
		shader.CompileFromFile(filename, entryPoint);
		SUCCESS_LOG(L"Pixel Shader Compiled");
		return shader;
	}
    
	PixelShader::PixelShader(const std::wstring& filepath, const std::wstring& entrypoint) : Shader(ShaderType::PixelShader)
	{
        this->filepath = filepath;
        this->entrypoint = entrypoint;
	}
    
}

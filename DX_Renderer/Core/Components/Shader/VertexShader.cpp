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
}

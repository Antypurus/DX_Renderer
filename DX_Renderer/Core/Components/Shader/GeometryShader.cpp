#include "GeometryShader.hpp"
#include "../../../Tooling/Log.hpp"

namespace DXR
{
	GeometryShader GeometryShader::CompileShaderFromFile(const std::wstring& filename, const std::string& entryPoint)
	{
		GeometryShader shader;
		INFO_LOG(L"Started Compiling Geometry Shader");
		shader.CompileFromFile(filename, entryPoint);
		SUCCESS_LOG(L"Geomtry Shader Compiled");
		return shader;
	}

	GeometryShader GeometryShader::CompileShader(const std::string& shaderCode, const std::string& entryPoint)
	{
		GeometryShader shader;
		INFO_LOG(L"Started Compiling Geometry Shader");
		shader.Compile(shaderCode, entryPoint);
		SUCCESS_LOG(L"Geomtry Shader Compiled");
		return shader;
	}

	GeometryShader::GeometryShader() :Shader(ShaderType::GeometryShader)
	{
	}

}

#pragma once
#include "Shader.hpp"

namespace DXR
{
	struct GeometryShader: public Shader
	{
	public:
	private:
	public:
		static GeometryShader CompileShaderFromFile(const std::wstring& filename, const std::string& entryPoint);
		static GeometryShader CompileShader(const std::string& shaderCode, const std::string& entryPoint);
	private:
		GeometryShader();
	};
}

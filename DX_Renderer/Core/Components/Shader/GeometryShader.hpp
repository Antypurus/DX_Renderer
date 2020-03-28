#pragma once
#include "Shader.hpp"

namespace DXR
{
	struct GeometryShader: public Shader
	{
	public:
	private:
	public:
		static GeometryShader CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint);
		static GeometryShader CompileShader(const std::wstring& shaderCode, const std::wstring& entryPoint);
	private:
		GeometryShader();
	};
}

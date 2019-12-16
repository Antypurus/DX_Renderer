#pragma once
#include "Shader.hpp"

namespace DXR
{
	struct PixelShader: public Shader
	{
	public:
	private:
	public:
		static PixelShader CompileShaderFromFile(const std::wstring& filename, const std::string& entryPoint);
		static PixelShader CompileShader(const std::string& shaderCode, const std::string& entryPoint);
	private:
		PixelShader();
	};
}

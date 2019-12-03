#pragma once
#include "Shader.hpp"

namespace DXR
{
	class PixelShader: public Shader
	{
	public:
	private:
	public:
		static PixelShader CompileFromFile(const std::wstring& filename, const std::string& entryPoint);
		static PixelShader Compile(const std::string& shaderCode, const std::string& entryPoint);
	private:
		PixelShader();
	};
}

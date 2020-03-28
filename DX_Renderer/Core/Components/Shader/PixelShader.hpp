#pragma once
#include "Shader.hpp"

namespace DXR
{
	struct PixelShader: public Shader
	{
	public:
	private:
	public:
		static PixelShader CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint);
	private:
		PixelShader();
	};
}

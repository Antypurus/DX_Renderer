#pragma once
#include "Shader.hpp"

namespace DXR
{	
	struct VertexShader: public Shader
	{
	public:
	private:
	public:
		static VertexShader CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint);
	protected:
		VertexShader();
	};
	
}

#pragma once
#include "Shader.hpp"

namespace DXR
{	
	struct VertexShader: public Shader
	{
	public:
	private:
	public:
		static VertexShader CompileShaderFromFile(const std::wstring& filename, const std::string& entryPoint);
		static VertexShader CompileShader(const std::string& shaderCode, const std::string& entryPoint);
	protected:
		VertexShader();
	};
	
}

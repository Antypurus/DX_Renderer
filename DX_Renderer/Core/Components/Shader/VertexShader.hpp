#pragma once
#include "Shader.hpp"

namespace DXR
{

#define SHADER_TYPE_NAME "vs_5_1"
	
	class VertexShader: public Shader
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

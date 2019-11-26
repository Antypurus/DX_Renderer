#pragma once
#include "Shader.hpp"

namespace DXR
{
	
	class VertexShader: public Shader
	{
	public:
	private:
	public:
		static VertexShader CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint);
		static VertexShader CompileShadert(const std::wstring& shaderCode, const std::wstring& entryPoint);
	protected:
		VertexShader();
		ID3DBlob* CompileFromFile(const std::wstring& filename, const std::string& entryPoint) override;
		ID3DBlob* Compile(const std::string& shaderCode, const std::string& entryPoint) override;
	};
	
}

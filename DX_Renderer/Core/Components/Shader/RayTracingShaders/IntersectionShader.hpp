#pragma once
#include "../Shader.hpp"

namespace DXR
{
	struct IntersectionShader: public Shader
	{
	public:
	private:
		std::wstring filepath;
		std::wstring entryPoint;
		std::wstring UniqueID;
	public:
		static IntersectionShader CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint);
		D3D12_STATE_SUBOBJECT GenerateShaderExport();
		std::wstring& GetUniqueID();
	private:
		IntersectionShader(const std::wstring& filepath,const std::wstring& entryPoint);
	};
}
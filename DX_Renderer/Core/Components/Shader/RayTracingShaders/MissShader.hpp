#pragma once
#include "../Shader.hpp"

namespace DXR
{
	struct MissShader: public Shader
	{
	public:
	private:
		std::wstring filepath;
		std::wstring entryPoint;
		std::wstring UniqueID;
	public:
		static MissShader CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint);
		D3D12_STATE_SUBOBJECT GenerateShaderExport();
		std::wstring& GetUniqueID();
	private:
		MissShader(const std::wstring& filepath,const std::wstring& entryPoint);
	};
}
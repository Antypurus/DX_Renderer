#pragma  once
#include "../Shader.hpp"

namespace DXR
{
	struct AnyHitShader : public Shader
	{
	public:
	private:
		std::wstring filepath;
		std::wstring entryPoint;
		std::wstring UniqueID;
	public:
		static AnyHitShader CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint);
		D3D12_STATE_SUBOBJECT GenerateShaderExport();
	private:
		AnyHitShader(const std::wstring& filepath, const std::wstring& entryPoint);
	};
}
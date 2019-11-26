#pragma once
#include <string>

#include <d3d12.h>
#include <wrl.h>

namespace DXR
{

	using namespace Microsoft;
	
	enum class ShaderType
	{
		None = -1,
		VertexShader = 0,
		HullShader = 1,
		DomainShader = 2,
		GeometryShader = 3,
		PixelShader = 4,
		ComputerShader = 5
	};
	
	class Shader
	{
	public:
	private:
		WRL::ComPtr<ID3DBlob> m_shader_code;
		ShaderType m_shader_type =  ShaderType::None;
	public:
		static Shader Compile(const std::string& filename);
	private:
	};
}

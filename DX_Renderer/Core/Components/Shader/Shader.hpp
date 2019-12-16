#pragma once
#include <string>

#include <d3d12.h>
#include <wrl.h>

namespace DXR
{

	using namespace Microsoft;

#define VERTEX_SHADER "vs_5_1"
#define HULL_SHADER "hs_5_1"
#define DOMAIN_SHADER "ds_5_1"
#define GEOMETRY_SHADER "gs_5_1"
#define PIXEL_SHADER "ps_5_1"
#define COMPUTE_SHADER "cs_5_1"

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

	struct Shader
	{
	public:
		virtual ~Shader() = default;
	protected:
		WRL::ComPtr<ID3DBlob> m_shader_code;
		ShaderType m_shader_type = ShaderType::None;
		std::string m_shader_type_name = "";
	public:
		D3D12_SHADER_BYTECODE GetShaderBytecode();
		static D3D12_SHADER_BYTECODE NoShaderBytecode(); 
	protected:
		virtual void CompileFromFile(const std::wstring& filename, const std::string& entryPoint);
		virtual void Compile(const std::string& shaderCode, const std::string& entryPoint);
		Shader(ShaderType shaderType);
	};
}

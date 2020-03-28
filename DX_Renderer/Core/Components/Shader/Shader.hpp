#pragma once
#include <string>

#include <d3d12.h>
#include <wrl.h>
     
#include <dxcapi.h>

namespace DXR
{

	using namespace Microsoft;

#define VERTEX_SHADER		L"vs_6_4"
#define HULL_SHADER			L"hs_6_4"
#define DOMAIN_SHADER		L"ds_6_4"
#define GEOMETRY_SHADER		L"gs_6_4"
#define PIXEL_SHADER		L"ps_6_4"
#define COMPUTE_SHADER		L"cs_6_4"

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
		WRL::ComPtr<IDxcBlob> m_shader_code;
		ShaderType m_shader_type = ShaderType::None;
		std::wstring m_shader_type_name = L"";
	public:
		D3D12_SHADER_BYTECODE GetShaderBytecode();
		static D3D12_SHADER_BYTECODE NoShaderBytecode(); 
	protected:
		Shader(ShaderType shaderType);
		virtual void CompileFromFile(const std::wstring& filename, const std::wstring& entryPoint);
	};

	
	struct ShaderCompiler
	{
	private:
		const static uint32_t m_shader_encoding = CP_UTF8;
		static ShaderCompiler m_instance;
		WRL::ComPtr<IDxcCompiler2> m_compiler;
		WRL::ComPtr<IDxcLibrary> m_library;
	public:
		static ShaderCompiler GetInstance();
		static IDxcBlob* CompileFromFile(const std::wstring& Filepath, const std::wstring& Entrypoint, const std::wstring ShaderType);
	private:
		ShaderCompiler();
	};
}

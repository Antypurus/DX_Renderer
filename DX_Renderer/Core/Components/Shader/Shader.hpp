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
#define RAYTRACING_SHADER	L"lib_6_4"
    
#define DXC_ARG_DEBUG L"-Zi"
#define DXC_ARG_SKIP_VALIDATION L"-Vd"
#define DXC_ARG_SKIP_OPTIMIZATIONS L"-Od"
#define DXC_ARG_PACK_MATRIX_ROW_MAJOR L"-Zpr"
#define DXC_ARG_PACK_MATRIX_COLUMN_MAJOR L"-Zpc"
#define DXC_ARG_AVOID_FLOW_CONTROL L"-Gfa"
#define DXC_ARG_PREFER_FLOW_CONTROL L"-Gfp"
#define DXC_ARG_ENABLE_STRICTNESS L"-Ges"
#define DXC_ARG_ENABLE_BACKWARDS_COMPATIBILITY L"-Gec"
#define DXC_ARG_IEEE_STRICTNESS L"-Gis"
#define DXC_ARG_OPTIMIZATION_LEVEL0 L"-O0"
#define DXC_ARG_OPTIMIZATION_LEVEL1 L"-O1"
#define DXC_ARG_OPTIMIZATION_LEVEL2 L"-O2"
#define DXC_ARG_OPTIMIZATION_LEVEL3 L"-O3"
#define DXC_ARG_WARNINGS_ARE_ERRORS L"-WX"
#define DXC_ARG_RESOURCES_MAY_ALIAS L"-res_may_alias"
#define DXC_ARG_ALL_RESOURCES_BOUND L"-all_resources_bound"
#define DXC_ARG_DEBUG_NAME_FOR_SOURCE L"-Zss"
#define DXC_ARG_DEBUG_NAME_FOR_BINARY L"-Zsb"

	enum class ShaderType
	{
		None = -1,
		VertexShader = 0,
		HullShader = 1,
		DomainShader = 2,
		GeometryShader = 3,
		PixelShader = 4,
		ComputerShader = 5,
		RayTracingShader = 6
	};
    
	struct Shader
	{
        public:
		virtual ~Shader() = default;
        protected:
		WRL::ComPtr<IDxcBlob> m_shader_code = nullptr;
		ShaderType m_shader_type = ShaderType::None;
		std::wstring m_shader_type_name = L"";
        public:
        Shader() = default;
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

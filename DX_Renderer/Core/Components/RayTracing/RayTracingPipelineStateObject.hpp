#pragma once

#include <d3d12.h>
#include <wrl.h>
#include "../Shader/RayTracingShaders/RayGenShader.hpp"
#include "../Shader/RayTracingShaders/IntersectionShader.hpp"
#include "../Shader/RayTracingShaders/AnyHitShader.hpp"
#include "../Shader/RayTracingShaders/ClosestHitShader.hpp"
#include "../Shader/RayTracingShaders/MissShader.hpp"
#include "../Shader/Root Signature/RootSignature.hpp"

namespace DXR
{
	using namespace Microsoft::WRL;

	struct GraphicsDevice;

	struct RayTracingPipelineStateObject
	{
	public:
		const static UINT PayloadMaxSize = 64 * 1024;//64K For Payload Size
		const static UINT MaxRayBounces = 1;
	private:
		ComPtr<ID3D12StateObject> m_rtpso = nullptr;
		ComPtr<ID3D12StateObjectProperties> m_rtpso_properties = nullptr;
		RayGenShader* m_ray_gen_shader;
		IntersectionShader* m_intersection_shader;
		AnyHitShader* m_any_hit_shader;
		ClosestHitShader* m_closest_hit_shader;
		MissShader* m_miss_shader;
		RootSignature* m_root_signature;

		//SUBOBJECTS IN MEMORY
		D3D12_RAYTRACING_SHADER_CONFIG shader_config_desc = {};
		D3D12_STATE_SUBOBJECT shader_config = {};

		D3D12_HIT_GROUP_DESC hit_group_desc = {};
		D3D12_STATE_SUBOBJECT hit_group = {};

		//For Now We Assume that all shaders use the paylaod
		const wchar_t* ShaderExports[3];

		D3D12_RAYTRACING_PIPELINE_CONFIG config_desc = {};
		D3D12_STATE_SUBOBJECT pipeline_config = {};

		D3D12_GLOBAL_ROOT_SIGNATURE global_rs_desc = {};
		D3D12_STATE_SUBOBJECT root_signature = {};

		D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION rs_association_desc = {};
		D3D12_STATE_SUBOBJECT rs_association = {};

		D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION shader_config_association_desc = {};
		D3D12_STATE_SUBOBJECT shader_config_association = {};

		D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION pipeline_association_desc = {};
		D3D12_STATE_SUBOBJECT pipeline_association = {};
	public:
		RayTracingPipelineStateObject(GraphicsDevice& Device, RootSignature& rootSignature, RayGenShader& raygenShader, IntersectionShader& intersectionShader, AnyHitShader& anyHitShader, ClosestHitShader& closestHitShader, MissShader& missShader);
		ID3D12StateObject* GetRTPSO();
	private:
		D3D12_STATE_SUBOBJECT CreateShaderConfiguration();
		D3D12_STATE_SUBOBJECT CreateShaderConfigAssociation();
		D3D12_STATE_SUBOBJECT CreateHitGroup();
		D3D12_STATE_SUBOBJECT CreatePipelineConfig();
		D3D12_STATE_SUBOBJECT CreatePipelineConfigAssociation();
		D3D12_STATE_SUBOBJECT CreateRootSignatureSubobject();
		D3D12_STATE_SUBOBJECT CreateRootSignatureAssociation();
	};
}

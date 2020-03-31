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
	public:
		RayTracingPipelineStateObject(GraphicsDevice& Device ,RootSignature& rootSignature, RayGenShader& raygenShader, IntersectionShader& intersectionShader, AnyHitShader& anyHitShader, ClosestHitShader& closestHitShader, MissShader& missShader);
	private:
		D3D12_STATE_SUBOBJECT CreateShaderConfiguration();
		D3D12_STATE_SUBOBJECT CreateHitGroup();
		D3D12_STATE_SUBOBJECT CreateShaderAssociation(D3D12_STATE_SUBOBJECT& ShaderConfig);
		D3D12_STATE_SUBOBJECT CreatePipelineConfig();
		D3D12_STATE_SUBOBJECT CreateRootSignatureAssociation();
	};
}

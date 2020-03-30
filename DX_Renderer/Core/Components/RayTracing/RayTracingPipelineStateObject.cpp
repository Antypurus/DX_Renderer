#include "RayTracingPipelineStateObject.hpp"

namespace DXR
{
	RayTracingPipelineStateObject::RayTracingPipelineStateObject(GraphicsDevice& Device, RootSignature& rootSignature, RayGenShader& raygenShader, IntersectionShader& intersectionShader, AnyHitShader& anyHitShader, ClosestHitShader& closestHitShader, MissShader& missShader)
	{
		this->m_ray_gen_shader = &raygenShader;
		this->m_intersection_shader =  &intersectionShader;
		this->m_any_hit_shader = &anyHitShader;
		this->m_closest_hit_shader = &closestHitShader;
		this->m_miss_shader = &missShader;
		this->m_root_signature = &rootSignature;


	}

	D3D12_STATE_SUBOBJECT RayTracingPipelineStateObject::CreateShaderConfiguration()
	{
		D3D12_RAYTRACING_SHADER_CONFIG shader_config_desc = {};
		shader_config_desc.MaxAttributeSizeInBytes = D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES;
		shader_config_desc.MaxPayloadSizeInBytes = RayTracingPipelineStateObject::PayloadMaxSize;

		D3D12_STATE_SUBOBJECT shader_config = {};
		shader_config.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
		shader_config.pDesc = &shader_config_desc;

		return shader_config;
	}
	D3D12_STATE_SUBOBJECT RayTracingPipelineStateObject::CreateHitGroup()
	{
		D3D12_HIT_GROUP_DESC hit_group_desc = {};
		hit_group_desc.AnyHitShaderImport = this->m_any_hit_shader->GetUniqueID().c_str();
		hit_group_desc.ClosestHitShaderImport = this->m_closest_hit_shader->GetUniqueID().c_str();
		hit_group_desc.IntersectionShaderImport = this->m_intersection_shader->GetUniqueID().c_str();
		hit_group_desc.HitGroupExport = L"HitGroup";

		D3D12_STATE_SUBOBJECT hit_group = {};
		hit_group.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
		hit_group.pDesc = &hit_group_desc;

		return hit_group;
	}
}

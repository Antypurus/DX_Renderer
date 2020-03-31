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

		ShaderExports[0] = this->m_ray_gen_shader->GetUniqueID().c_str();
		ShaderExports[1] = this->m_miss_shader->GetUniqueID().c_str();
		ShaderExports[2] = L"HitGroup";
	}

	D3D12_STATE_SUBOBJECT RayTracingPipelineStateObject::CreateShaderConfiguration()
	{
		shader_config_desc.MaxAttributeSizeInBytes = D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES;
		shader_config_desc.MaxPayloadSizeInBytes = RayTracingPipelineStateObject::PayloadMaxSize;

		shader_config.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
		shader_config.pDesc = &shader_config_desc;

		return shader_config;
	}

	D3D12_STATE_SUBOBJECT RayTracingPipelineStateObject::CreateHitGroup()
	{
		hit_group_desc.AnyHitShaderImport = this->m_any_hit_shader->GetUniqueID().c_str();
		hit_group_desc.ClosestHitShaderImport = this->m_closest_hit_shader->GetUniqueID().c_str();
		hit_group_desc.IntersectionShaderImport = this->m_intersection_shader->GetUniqueID().c_str();
		hit_group_desc.HitGroupExport = L"HitGroup";

		hit_group.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
		hit_group.pDesc = &hit_group_desc;

		return hit_group;
	}

	D3D12_STATE_SUBOBJECT RayTracingPipelineStateObject::CreateShaderAssociation(D3D12_STATE_SUBOBJECT& ShaderConfig)
	{
		export_associations_desc.NumExports = 3;
		export_associations_desc.pExports = ShaderExports;
		export_associations_desc.pSubobjectToAssociate = &ShaderConfig;

		export_association.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
		export_association.pDesc = &export_associations_desc;

		return export_association;
	}

	D3D12_STATE_SUBOBJECT RayTracingPipelineStateObject::CreatePipelineConfig()
	{
		config_desc.MaxTraceRecursionDepth = RayTracingPipelineStateObject::MaxRayBounces;

		pipeline_config.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
		pipeline_config.pDesc = &config_desc;

		return pipeline_config;
	}

	D3D12_STATE_SUBOBJECT RayTracingPipelineStateObject::CreateRootSignatureSubobject()
	{
		global_rs_desc.pGlobalRootSignature = this->m_root_signature->GetRootSignature();
		
		root_signature.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
		root_signature.pDesc = &global_rs_desc;
		
		return root_signature;
	}

	D3D12_STATE_SUBOBJECT RayTracingPipelineStateObject::CreateRootSignatureAssociation()
	{
		rs_association_desc.NumExports = 3;
		rs_association_desc.pExports = ShaderExports;
		rs_association_desc.pSubobjectToAssociate = &root_signature;

		rs_association.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
		rs_association.pDesc = &rs_association_desc;

		return rs_association;
	}


}

#include "RayTracingPipelineStateObject.hpp"
#include "../GraphicsDevice.hpp"
#include "../../../Tooling/Validate.hpp"
#include "../../../ThirdParty/NVAPI/nvapi.h"

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
        
		D3D12_STATE_SUBOBJECT subobjects[10];
		subobjects[0] = this->m_ray_gen_shader->GenerateShaderExport();
		//subobjects[1] = this->m_intersection_shader->GenerateShaderExport();
		//subobjects[2] = this->m_any_hit_shader->GenerateShaderExport();
		subobjects[1] = this->m_closest_hit_shader->GenerateShaderExport();
		subobjects[2] = this->m_miss_shader->GenerateShaderExport();
		subobjects[3] = CreateShaderConfiguration();
		subobjects[4] = CreateHitGroup();
		subobjects[5] = CreateShaderConfigAssociation();
		subobjects[6] = CreatePipelineConfig();
		subobjects[7] = CreateRootSignatureSubobject();
		subobjects[8] = CreateRootSignatureAssociation();
		subobjects[9] = CreatePipelineConfigAssociation();
        
		D3D12_STATE_OBJECT_DESC rtpso_desc = {};
		rtpso_desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
		rtpso_desc.NumSubobjects = 10;
		rtpso_desc.pSubobjects = subobjects;
        
        NvAPI_Status status = NvAPI_D3D12_SetNvShaderExtnSlotSpace(Device.GetRawInterface(),10,0);
        
        if(status != NVAPI_OK)
        {
            ERROR_LOG(L"Failed Creation Of NVAPI Fake UAV");
        }
        
		DXCall(Device->CreateStateObject(&rtpso_desc,IID_PPV_ARGS(&this->m_rtpso)));
		DXCall(this->m_rtpso->QueryInterface(IID_PPV_ARGS(&this->m_rtpso_properties)));
        
        status = NvAPI_D3D12_SetNvShaderExtnSlotSpace(Device.GetRawInterface(),~0u,0);
        
        if(status != NVAPI_OK)
        {
            ERROR_LOG(L"Failed Unsetting Of NVAPI Fake UAV");
        }
	}
    
	ID3D12StateObject* RayTracingPipelineStateObject::GetRTPSO()
	{
		return this->m_rtpso.Get();
	}
    
	ID3D12StateObjectProperties* RayTracingPipelineStateObject::GetRTPSOInfo()
	{
		return this->m_rtpso_properties.Get();
	}
    
	D3D12_STATE_SUBOBJECT RayTracingPipelineStateObject::CreateShaderConfiguration()
	{
		shader_config_desc.MaxAttributeSizeInBytes = D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES;
		shader_config_desc.MaxPayloadSizeInBytes = RayTracingPipelineStateObject::PayloadMaxSize;
        
		shader_config.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
		shader_config.pDesc = &shader_config_desc;
        
		return shader_config;
	}
    
	D3D12_STATE_SUBOBJECT RayTracingPipelineStateObject::CreateShaderConfigAssociation()
	{
		shader_config_association_desc.NumExports = 3;
		shader_config_association_desc.pExports = this->ShaderExports;
		shader_config_association_desc.pSubobjectToAssociate = &this->shader_config;
        
		shader_config_association.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
		shader_config_association.pDesc = &shader_config_association_desc;
        
		return shader_config_association;
	}
    
	D3D12_STATE_SUBOBJECT RayTracingPipelineStateObject::CreateHitGroup()
	{
		//hit_group_desc.AnyHitShaderImport = this->m_any_hit_shader->GetUniqueID().c_str();
		hit_group_desc.ClosestHitShaderImport = this->m_closest_hit_shader->GetUniqueID().c_str();
		//hit_group_desc.IntersectionShaderImport = this->m_intersection_shader->GetUniqueID().c_str();
		hit_group_desc.HitGroupExport = L"HitGroup";
		//hit_group_desc.Type = D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE;
        
		hit_group.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
		hit_group.pDesc = &hit_group_desc;
        
		return hit_group;
	}
    
	D3D12_STATE_SUBOBJECT RayTracingPipelineStateObject::CreatePipelineConfig()
	{
		config_desc.MaxTraceRecursionDepth = RayTracingPipelineStateObject::MaxRayBounces;
        
		pipeline_config.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
		pipeline_config.pDesc = &config_desc;
        
		return pipeline_config;
	}
    
	D3D12_STATE_SUBOBJECT RayTracingPipelineStateObject::CreatePipelineConfigAssociation()
	{
		pipeline_association_desc.NumExports = 3;
		pipeline_association_desc.pExports = ShaderExports;
		pipeline_association_desc.pSubobjectToAssociate = &pipeline_config;
        
		pipeline_association.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
		pipeline_association.pDesc = &pipeline_association_desc;
        
		return pipeline_association;
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

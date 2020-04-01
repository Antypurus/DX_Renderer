#include "ShaderBindingTable.hpp"
#include "../Shader/RayTracingShaders/RayGenShader.hpp"
#include "../Shader/RayTracingShaders/MissShader.hpp"
#include "../GraphicsDevice.hpp"

namespace DXR
{
	RayGenSBTEntry::RayGenSBTEntry(RayGenShader& Shader)
	{
		this->shader = shader;
	}

	void RayGenSBTEntry::AddResource(D3D12_GPU_DESCRIPTOR_HANDLE& Handle)
	{
		this->resources.push_back(Handle);
	}

	UINT RayGenSBTEntry::CalculateEntrySize()
	{
		UINT shader_id_size = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		UINT resource_count = this->resources.size();
		UINT resource_size = resource_count * 8;
		return Align(shader_id_size + resource_size, 16);
	}

	MissSBTEntry::MissSBTEntry(MissShader& Shader)
	{
		this->shader = shader;
	}

	void MissSBTEntry::AddResource(D3D12_GPU_DESCRIPTOR_HANDLE& Handle)
	{
		this->resources.push_back(Handle);
	}

	UINT MissSBTEntry::CalculateEntrySize()
	{
		UINT shader_id_size = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		UINT resource_count = this->resources.size();
		UINT resource_size = resource_count * 8;
		return Align(shader_id_size + resource_size, 16);
	}

	HitGroupSBTEntry::HitGroupSBTEntry(const std::wstring& HitGroupName)
	{
		this->HitGroupName = HitGroupName;
	}

	void HitGroupSBTEntry::AddResource(D3D12_GPU_DESCRIPTOR_HANDLE& Handle)
	{
		this->resources.push_back(Handle);
	}

	UINT HitGroupSBTEntry::CalculateEntrySize()
	{
		UINT shader_id_size = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		UINT resource_count = this->resources.size();
		UINT resource_size = resource_count * 8;
		return Align(shader_id_size + resource_size, 16);
	}

	ShaderBindingTable::ShaderBindingTable(GraphicsDevice& Device, RayTracingPipelineStateObject& RTpso, RayGenSBTEntry& raygen, MissSBTEntry& Miss, HitGroupSBTEntry& HitGroup)
	{
		this->RGS = &raygen;
		this->HG = &HitGroup;
		this->MS = &Miss;
		this->CalculateTableSize();
	}

	void ShaderBindingTable::CalculateTableSize()
	{
		UINT rgs_size = this->RGS->CalculateEntrySize();
		UINT hg_size = this->HG->CalculateEntrySize();
		UINT ms_size = this->MS->CalculateEntrySize();

		this->table_size = Align(rgs_size+hg_size+ms_size,256);
	}
}
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

	void RayGenSBTEntry::CopyDataToBuffer(uint8_t* buffer, RayTracingPipelineStateObject& rtpso)
	{
		UINT shader_id_size = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

		void* shader_id = rtpso.GetRTPSOInfo()->GetShaderIdentifier(shader->GetUniqueID().c_str());
		memcpy(buffer,shader_id,shader_id_size);

		memcpy(buffer+shader_id_size,this->resources.data(),this->resources.size() * 8);
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

	void MissSBTEntry::CopyDataToBuffer(uint8_t* buffer, RayTracingPipelineStateObject& rtpso)
	{
		UINT shader_id_size = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

		void* shader_id = rtpso.GetRTPSOInfo()->GetShaderIdentifier(shader->GetUniqueID().c_str());
		memcpy(buffer, shader_id, shader_id_size);

		memcpy(buffer + shader_id_size, this->resources.data(), this->resources.size() * 8);
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

	void HitGroupSBTEntry::CopyDataToBuffer(uint8_t* buffer, RayTracingPipelineStateObject& rtpso)
	{
		UINT shader_id_size = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

		void* shader_id = rtpso.GetRTPSOInfo()->GetShaderIdentifier(this->HitGroupName.c_str());
		memcpy(buffer, shader_id, shader_id_size);

		memcpy(buffer + shader_id_size, this->resources.data(), this->resources.size() * 8);
	}

	ShaderBindingTable::ShaderBindingTable(GraphicsDevice& Device, RayTracingPipelineStateObject& RTpso, RayGenSBTEntry& raygen, MissSBTEntry& Miss, HitGroupSBTEntry& HitGroup)
	{
		this->RGS = &raygen;
		this->HG = &HitGroup;
		this->MS = &Miss;
		this->CalculateTableSize();
		this->AllocateDataBuffer();
	}

	void ShaderBindingTable::CalculateTableSize()
	{
		UINT rgs_size = this->RGS->CalculateEntrySize();
		UINT hg_size = this->HG->CalculateEntrySize();
		UINT ms_size = this->MS->CalculateEntrySize();

		this->table_size = Align(rgs_size+hg_size+ms_size,256);
	}

	void ShaderBindingTable::AllocateDataBuffer()
	{
		this->m_data_buffer = std::make_unique<uint8_t[]>(this->table_size);
	}

	void ShaderBindingTable::CopyTableDataToBuffer(RayTracingPipelineStateObject& RTpso)
	{
		UINT64 offset = 0;
		this->RGS->CopyDataToBuffer(this->m_data_buffer.get(),RTpso);

		offset += this->RGS->CalculateEntrySize();
		this->MS->CopyDataToBuffer(this->m_data_buffer.get()+offset,RTpso);

		offset += this->MS->CalculateEntrySize();
		this->HG->CopyDataToBuffer(this->m_data_buffer.get()+offset,RTpso);
	}
}
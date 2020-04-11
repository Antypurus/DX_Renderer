#include "ShaderBindingTable.hpp"
#include "../GraphicsDevice.hpp"
#include "RayTracingPipelineStateObject.hpp"

namespace DXR
{
	RayGenSBTEntry::RayGenSBTEntry(RayGenShader& Shader)
	{
		this->shader = &Shader;
	}

	void RayGenSBTEntry::AddResource(D3D12_GPU_DESCRIPTOR_HANDLE& Handle)
	{
		this->resources.push_back(Handle);
	}

	UINT RayGenSBTEntry::CalculateEntrySize()
	{
		UINT shader_id_size = Align(D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES,32);
		UINT resource_count = this->resources.size();
		UINT resource_size = resource_count * 8;
		return Align(shader_id_size + resource_size, 32);
	}

	void RayGenSBTEntry::CopyDataToBuffer(uint8_t* buffer, RayTracingPipelineStateObject& rtpso)
	{
		UINT shader_id_size = Align(D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES,64);

		void* shader_id = rtpso.GetRTPSOInfo()->GetShaderIdentifier(shader->GetUniqueID().c_str());
		memcpy(buffer,shader_id,shader_id_size);

		memcpy(buffer+shader_id_size,this->resources.data(),this->resources.size() * 8);
	}

	MissSBTEntry::MissSBTEntry(MissShader& Shader)
	{
		this->shader = &Shader;
	}

	void MissSBTEntry::AddResource(D3D12_GPU_DESCRIPTOR_HANDLE& Handle)
	{
		this->resources.push_back(Handle);
	}

	UINT MissSBTEntry::CalculateEntrySize()
	{
		UINT shader_id_size = Align(D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES,64);
		UINT resource_count = this->resources.size();
		UINT resource_size = resource_count * 8;
		return Align(shader_id_size + resource_size, 32);
	}

	void MissSBTEntry::CopyDataToBuffer(uint8_t* buffer, RayTracingPipelineStateObject& rtpso)
	{
		UINT shader_id_size = Align(D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES,64);

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
		UINT shader_id_size = Align(D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES,64);
		UINT resource_count = this->resources.size();
		UINT resource_size = resource_count * 8;
		return Align(shader_id_size + resource_size, 32);
	}

	void HitGroupSBTEntry::CopyDataToBuffer(uint8_t* buffer, RayTracingPipelineStateObject& rtpso)
	{
		UINT shader_id_size = Align(D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES,64);

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
		this->CopyTableDataToBuffer(RTpso);
		this->AllocateAndCopyGPUBuffer(Device);
	}

	UINT ShaderBindingTable::GetRayGenSectionSize()
	{
		return this->RGS->CalculateEntrySize();
	}

	UINT ShaderBindingTable::GetRayGenEntrySize()
	{
		return this->RGS->CalculateEntrySize();
	}

	UINT ShaderBindingTable::GetMissSectionSize()
	{
		return this->HG->CalculateEntrySize();
	}

	UINT ShaderBindingTable::GetMissEntrySize()
	{
		return this->HG->CalculateEntrySize();
	}

	UINT ShaderBindingTable::GetHitGroupSectionSize()
	{
		return this->MS->CalculateEntrySize();
	}

	UINT ShaderBindingTable::GetHitGroupEntrySize()
	{
		return this->MS->CalculateEntrySize();
	}

	D3D12_GPU_VIRTUAL_ADDRESS ShaderBindingTable::GetRayGenEntryAddress()
	{
		return this->m_table_buffer->GetGPUAddress();
	}

	D3D12_GPU_VIRTUAL_ADDRESS ShaderBindingTable::GetMissEntryAddress()
	{
		return this->m_table_buffer->GetGPUAddress() + this->RGS->CalculateEntrySize();
	}

	D3D12_GPU_VIRTUAL_ADDRESS ShaderBindingTable::GetHitGroupEntryAddress()
	{
		return this->m_table_buffer->GetGPUAddress() + this->RGS->CalculateEntrySize() + this->MS->CalculateEntrySize();
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

	void ShaderBindingTable::AllocateAndCopyGPUBuffer(GraphicsDevice& device)
	{
		this->m_table_buffer = std::make_unique<GPUUploadBuffer>(device,1,this->table_size,this->m_data_buffer.get());
		this->m_table_buffer->GetResource()->SetName(L"Shader Binding Table");
	}
}
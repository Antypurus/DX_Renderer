#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include <d3d12.h>
#include "../Resource/GPU Buffers/GPUUploadBuffer.hpp"
#include "../Shader/RayTracingShaders/RayGenShader.hpp"
#include "../Shader/RayTracingShaders/MissShader.hpp"


namespace DXR
{
	struct RayTracingPipelineStateObject;
	struct ShaderBindingTable;

	static UINT Align(UINT Original, UINT AlignTo)
	{
		return std::lround(std::ceil(Original / (double)AlignTo)) * AlignTo;
	}

	struct RayGenSBTEntry
	{
	private:
		friend ShaderBindingTable;

		RayGenShader* shader;
		std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> resources;
	public:
		RayGenSBTEntry(RayGenShader& Shader);
		void AddResource(D3D12_GPU_DESCRIPTOR_HANDLE& Handle);
		UINT CalculateEntrySize();
		void CopyDataToBuffer(uint8_t* buffer,RayTracingPipelineStateObject& rtpso);
	};

	struct MissSBTEntry
	{
	private:
		friend ShaderBindingTable;

		MissShader* shader;
		std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> resources;
	public:
		MissSBTEntry(MissShader& Shader);
		void AddResource(D3D12_GPU_DESCRIPTOR_HANDLE& Handle);
		UINT CalculateEntrySize();
		void CopyDataToBuffer(uint8_t* buffer,RayTracingPipelineStateObject& rtpso);
	};

	struct HitGroupSBTEntry
	{
	private:
		friend ShaderBindingTable;

		std::wstring HitGroupName;
		std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> resources;
	public:
		HitGroupSBTEntry(const std::wstring& HitGroupName);
		void AddResource(D3D12_GPU_DESCRIPTOR_HANDLE& Handle);
		UINT CalculateEntrySize();
		void CopyDataToBuffer(uint8_t* buffer,RayTracingPipelineStateObject& rtpso);
	};

	struct ShaderBindingTable
	{
	private:
		RayGenSBTEntry* RGS;
		MissSBTEntry* MS;
		HitGroupSBTEntry* HG;
		UINT64 table_size = 0;
		std::unique_ptr<uint8_t[]> m_data_buffer;
		std::unique_ptr<GPUUploadBuffer> m_table_buffer;
	public:
		ShaderBindingTable(GraphicsDevice& Device, RayTracingPipelineStateObject& RTpso, RayGenSBTEntry& raygen, MissSBTEntry& Miss, HitGroupSBTEntry& HitGroup);
		UINT GetRayGenSectionSize();
		UINT GetRayGenEntrySize();
		UINT GetMissSectionSize();
		UINT GetMissEntrySize();
		UINT GetHitGroupSectionSize();
		UINT GetHitGroupEntrySize();
		D3D12_GPU_VIRTUAL_ADDRESS GetRayGenEntryAddress();
		D3D12_GPU_VIRTUAL_ADDRESS GetMissEntryAddress();
		D3D12_GPU_VIRTUAL_ADDRESS GetHitGroupEntryAddress();
	private:
		void CalculateTableSize();
		void AllocateDataBuffer();
		void CopyTableDataToBuffer(RayTracingPipelineStateObject& RTpso);
		void AllocateAndCopyGPUBuffer(GraphicsDevice& device);
	};

}
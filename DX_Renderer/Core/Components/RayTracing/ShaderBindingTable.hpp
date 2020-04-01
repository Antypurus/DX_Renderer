#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include <d3d12.h>
#include "RayTracingPipelineStateObject.hpp"

namespace DXR
{
	struct RayGenShader;
	struct MissShader;
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
	};

	struct MissSBTEntry
	{
	private:
		MissShader* shader;
		std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> resources;
	public:
		MissSBTEntry(RayGenShader& Shader);
		void AddResource(D3D12_GPU_DESCRIPTOR_HANDLE& Handle);
		UINT CalculateEntrySize();
	};

	struct HitGroupSBTEntry
	{
	private:
		std::wstring HitGroupName;
		std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> resources;
	public:
		HitGroupSBTEntry(const std::wstring& HitGroupName);
		void AddResource(D3D12_GPU_DESCRIPTOR_HANDLE& Handle);
		UINT CalculateEntrySize();
	};

	struct ShaderBindingTable
	{
	};

}
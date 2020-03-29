#include "../GraphicsDevice.hpp"
#include "AccelerationStructure.hpp"
#include  "../Resource/GPU Buffers/GPUDefaultBuffer.hpp"
#include "../Command List/GraphicsCommandList.hpp"

namespace DXR
{
	void BLAS::BuildBLAS(GraphicsDevice& Device, GraphicsCommandList& commandList)
	{
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS blas = {};
		blas.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		blas.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		blas.pGeometryDescs = &this->m_blas_descripiton;
		blas.NumDescs = 1;
		blas.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

		//Query the GPU for the memory requirements
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO blas_mem_requirements;
		Device->GetRaytracingAccelerationStructurePrebuildInfo(&blas, &blas_mem_requirements);

		this->m_scratch_buffer = std::make_unique<GPUDefaultBuffer>(Device, commandList, 1, blas_mem_requirements.ScratchDataSizeInBytes, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	}
}

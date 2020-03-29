#include "../GraphicsDevice.hpp"
#include "AccelerationStructure.hpp"
#include  "../Resource/GPU Buffers/GPUDefaultBuffer.hpp"
#include "../Command List/GraphicsCommandList.hpp"

namespace DXR
{
	void BLAS::BuildBLAS(GraphicsDevice& Device, GraphicsCommandList& commandList)
	{
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS blas_inputs = {};
		blas_inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		blas_inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		blas_inputs.pGeometryDescs = &this->m_blas_descripiton;
		blas_inputs.NumDescs = 1;
		blas_inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

		//Query the GPU for the memory requirements
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO blas_mem_requirements;
		Device->GetRaytracingAccelerationStructurePrebuildInfo(&blas_inputs, &blas_mem_requirements);

		//Allocate the scratch and result buffers
		this->m_scratch_buffer = std::make_unique<GPUDefaultBuffer>(Device, commandList, 1, blas_mem_requirements.ScratchDataSizeInBytes, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		this->m_blas_buffer = std::make_unique<GPUDefaultBuffer>(Device, commandList, 1, blas_mem_requirements.ScratchDataSizeInBytes, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC blas = {};
		blas.Inputs = blas_inputs;
		blas.ScratchAccelerationStructureData = this->m_scratch_buffer->GetGPUAddress();
		blas.DestAccelerationStructureData = this->m_blas_buffer->GetGPUAddress();
		
		commandList->BuildRaytracingAccelerationStructure(&blas,0,nullptr);
	}
}

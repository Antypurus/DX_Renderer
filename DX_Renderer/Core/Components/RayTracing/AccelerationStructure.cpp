#include "../GraphicsDevice.hpp"
#include "AccelerationStructure.hpp"
#include  "../Resource/GPU Buffers/GPUDefaultBuffer.hpp"
#include "../Command List/GraphicsCommandList.hpp"
#include "../Resource/ResourceBarrier.hpp"

namespace DXR
{
	D3D12_GPU_VIRTUAL_ADDRESS BLAS::GetBLASGPUAddress() const
	{
		return this->m_blas_buffer->GetGPUAddress();
	}

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
		this->m_blas_buffer = std::make_unique<GPUDefaultBuffer>(Device, commandList, 1, blas_mem_requirements.ResultDataMaxSizeInBytes, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC blas = {};
		blas.Inputs = blas_inputs;
		blas.ScratchAccelerationStructureData = this->m_scratch_buffer->GetGPUAddress();
		blas.DestAccelerationStructureData = this->m_blas_buffer->GetGPUAddress();

		commandList->BuildRaytracingAccelerationStructure(&blas, 0, nullptr);

		//Barrier to wait for the buffer creation to finish
		UAVResourceBarrier barrier(*this->m_blas_buffer->GetResource());
		barrier.ExecuteResourceBarrier(commandList);
	}

	BLASInstance::BLASInstance(const BLAS& blas, const XMMATRIX& Transform, UINT HitGroup, UINT InstanceID)
	{
		memcpy(m_instance_description.Transform, &Transform, sizeof(Transform));
		m_instance_description.InstanceMask = 1;
		m_instance_description.InstanceID = InstanceID;
		m_instance_description.InstanceContributionToHitGroupIndex = HitGroup;
		m_instance_description.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
		m_instance_description.AccelerationStructure = blas.GetBLASGPUAddress();
	}

	D3D12_RAYTRACING_INSTANCE_DESC BLASInstance::GetInstanceDescription() const
	{
		return this->m_instance_description;
	}

	void TLAS::AddInstance(const BLAS& blas, const XMMATRIX& Transform, UINT HitGroup)
	{
		BLASInstance instance(blas, Transform, HitGroup, m_blas_instances.size());
		this->m_blas_instances.push_back(instance);
	}

	void TLAS::BuildTLAS(GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{
		D3D12_RAYTRACING_INSTANCE_DESC* instances = new D3D12_RAYTRACING_INSTANCE_DESC[this->m_blas_instances.size()];
		
		while (instances == nullptr)
		{
			instances = new D3D12_RAYTRACING_INSTANCE_DESC[this->m_blas_instances.size()];
		}

		for(size_t i=0;i<this->m_blas_instances.size();++i)
		{
			instances[i] = this->m_blas_instances[i].GetInstanceDescription();
		}

		//descriptors now need to be uploaded to GPU memory in an upload buffer
		this->m_instance_descriptor_buffer = std::make_unique<GPUUploadBuffer>(Device,this->m_blas_instances.size(),sizeof(D3D12_RAYTRACING_INSTANCE_DESC), instances);

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS tlas_inputs = {};
		tlas_inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		tlas_inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		tlas_inputs.InstanceDescs = this->m_instance_descriptor_buffer->GetGPUAddress();
		tlas_inputs.NumDescs = this->m_blas_instances.size();
		tlas_inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO tlas_mem_requirements;
		Device->GetRaytracingAccelerationStructurePrebuildInfo(&tlas_inputs, &tlas_mem_requirements);

		//Allocate the scratch and result buffers
		this->m_scratch_buffer = std::make_unique<GPUDefaultBuffer>(Device, CommandList, 1, tlas_mem_requirements.ScratchDataSizeInBytes, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		this->m_tlas_buffer = std::make_unique<GPUDefaultBuffer>(Device, CommandList, 1, tlas_mem_requirements.ResultDataMaxSizeInBytes, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		//actually build the tlas
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC tlas = {};
		tlas.Inputs = tlas_inputs;
		tlas.ScratchAccelerationStructureData = this->m_scratch_buffer->GetGPUAddress();
		tlas.DestAccelerationStructureData = this->m_tlas_buffer->GetGPUAddress();
		CommandList->BuildRaytracingAccelerationStructure(&tlas, 0, nullptr);

		//Barrier to wait for the buffer creation to finish
		UAVResourceBarrier barrier(*this->m_tlas_buffer->GetResource());
		barrier.ExecuteResourceBarrier(CommandList);
	}
}

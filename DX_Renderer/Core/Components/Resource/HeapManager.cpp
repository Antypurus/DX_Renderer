#include "HeapManager.hpp"
#include "../GraphicsDevice.hpp"
#include "../../../Tooling/Log.hpp"

namespace DXR
{
	SubHeapManager::SubHeapManager(const SubHeapManager& other)
	{
		uint32_t value = other.free_index_count;
		this->free_index_count = value;
		this->allocated_indices = other.allocated_indices;
		this->m_free_heap_indices = other.m_free_heap_indices;
	}

	SubHeapManager::SubHeapManager(const std::vector<uint32_t>& HeapIndices)
	{
		const size_t heap_size = HeapIndices.size();
		for (size_t index = 0; index < heap_size; ++index)
		{
			this->m_free_heap_indices.push_back(HeapIndices[heap_size - index - 1]);
		}
		this->free_index_count = HeapIndices.size();
	}

	/*
	  Note(Tiago): Only One Allocation In A Given Sub-Manager Can Be Happening At A Given Time
	  For This reason we need to lock the mutex at the start of this method and unlock before returning a value
	  -1 or 0xFF is returned if no free indices are available in this manager. This can create an issue if a manager was to old 32bits worth of indices however that should never happen.

	  Note(Tiago): Vector here could likely be substituted by a stack or queue
	*/
	uint32_t SubHeapManager::Allocate()
	{
		if (free_index_count <= 0) return 0xFF;//Note(Tiago): Log attempt to allocate on empty manager?

		uint32_t index = 0xFF;
		m_mutex.lock();

		this->free_index_count--;
		index = this->m_free_heap_indices[this->m_free_heap_indices.size() - 1];
		this->m_free_heap_indices.pop_back();

		this->allocated_indices[index] = true;

		m_mutex.unlock();
		return index;
	}

	void SubHeapManager::Free(uint32_t index)
	{
		m_mutex.lock();

		//validate that index belongs to heap
		auto allocation = this->allocated_indices.find(index);
		if (allocation == this->allocated_indices.end())
		{
			//Index does not belong to this manager
			WARNING_LOG("Attempt To Free Index That Is Not Managed By This Heap SubManager");
		}
		else
		{
			this->allocated_indices.erase(allocation);
		}

		this->m_free_heap_indices.push_back(index);
		this->free_index_count++;

		m_mutex.unlock();
	}

	HeapManager::HeapManager(UINT HeapSize, UINT SubManagerCount, DescriptorType Type)
	{
		this->heap_size = HeapSize;
		this->submanager_count = SubManagerCount;

		if (this->heap_size % this->submanager_count != 0)
		{
			ERROR_LOG("Heap Is Not Equaly Divisible By All Submanagers");
#ifndef NDEBUG
			__debugbreak();
#endif
			exit(-1);
		}

		if (Type == DescriptorType::ConstantBufferView)
		{
			this->descriptor_heap = GraphicsDevice::Device->CreateConstantBufferDescriptorHeap(this->heap_size);
		}
		else if (Type == DescriptorType::Sampler)
		{
			this->descriptor_heap = GraphicsDevice::Device->CreateSamplerDescriptorHeap(this->heap_size);
		}

		//Divide indices about the managers
		UINT indices_per_manager = this->heap_size / this->submanager_count;
		for (size_t manager = 0; manager < this->submanager_count; ++manager)
		{
			std::vector<uint32_t> indices;
			for (size_t i = 0; i < indices_per_manager; ++i)
			{
				indices.push_back(manager * indices_per_manager + i);
			}
			this->managers.emplace_back(indices);
		}
	}

	uint32_t HeapManager::Allocate()
	{
		UINT64 manager_index = next_manager++;
		manager_index %= submanager_count;
		return managers[manager_index].Allocate();
	}

	void HeapManager::Free(uint32_t index)
	{
		auto manager = this->index_mapping.find(index);
		if (manager == this->index_mapping.end())
		{
			//Out of Bound Descriptor Heap Index
			WARNING_LOG("Attempt To Free Out Of Bound Deacriptor Heap Index");
		}
		else
		{
			size_t manager_index = manager->second;
			this->managers[manager_index].Free(index);
		}
	}

	std::mutex SRHeapManager::mutex;
	SRHeapManager* SRHeapManager::Manager = nullptr;

	SRHeapManager& SRHeapManager::GetManager()
	{
		if (SRHeapManager::Manager == nullptr)
		{
			SRHeapManager::mutex.lock();

			if (SRHeapManager::Manager != nullptr) return *SRHeapManager::Manager;
			SRHeapManager::Manager = new SRHeapManager();//Note(Tiago): These managers must always be alive so its fine if they are not cleaned up

			SRHeapManager::mutex.unlock();
		}
		return *SRHeapManager::Manager;
	}

	SRHeapManager::SRHeapManager() :HeapManager(srv_uav_cbv_heap_size, submanager_ammount, DescriptorType::ConstantBufferView)
	{
	}

	std::mutex SamplerHeapManager::mutex;
	SamplerHeapManager* SamplerHeapManager::Manager = nullptr;

	SamplerHeapManager& SamplerHeapManager::GetManager()
	{
		if (SamplerHeapManager::Manager == nullptr)
		{
			SamplerHeapManager::mutex.lock();

			if (SamplerHeapManager::Manager != nullptr) return *SamplerHeapManager::Manager;
			SamplerHeapManager::Manager = new SamplerHeapManager();//Note(Tiago): These managers must always be alive so its fine if they are not cleaned up

			SamplerHeapManager::mutex.unlock();
		}
		return *SamplerHeapManager::Manager;
	}

	SamplerHeapManager::SamplerHeapManager() :HeapManager(sampler_heap_size, submanager_ammount, DescriptorType::Sampler)
	{
	}

}

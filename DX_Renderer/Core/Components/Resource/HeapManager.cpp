#include "HeapManager.hpp"
#include "../GraphicsDevice.hpp"
#include "../../../Tooling/Log.hpp"

namespace DXR
{

	SubHeapManager::SubHeapManager(const std::vector<uint32_t>& HeapIndices)
	{
		const size_t heap_size = HeapIndices.size();
		for(size_t index = 0;index < heap_size;++index)
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
		if(free_index_count <= 0) return 0xFF;//Note(Tiago): Log attempt to allocate on empty manager?

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
		if(allocation == this->allocated_indices.end())
		{
			//Index does not belong to this manager
			WARNING_LOG("Attempt To Free Index That Is Not Managed By This Heap SubManager");
		}else
		{
			this->allocated_indices.erase(allocation);
		}

		this->m_free_heap_indices.push_back(index);
		this->free_index_count++;

		m_mutex.unlock();
	}

	HeapManager::HeapManager(UINT HeapSize, UINT SubManagerCount)
	{
		this->heap_size = HeapSize;
		this->submanager_count = SubManagerCount;

		this->descriptor_heap = GraphicsDevice::Device->CreateConstantBufferDescriptorHeap(HeapSize);

		//Divide indices about the managers
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
		if(manager == this->index_mapping.end())
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

}

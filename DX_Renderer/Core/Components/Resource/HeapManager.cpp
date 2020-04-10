#include "HeapManager.hpp"

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

	  Note(Tiago): Vector here oculd likely be susbtituted by a stack or queue
	*/
	uint32_t SubHeapManager::Allocate()
	{
		if(free_index_count <= 0) return 0xFF;//Note(Tiago): Log attempt to allocate on empty manager?

		uint32_t index = 0xFF;
		m_mutex.lock();

		this->free_index_count--;
		index = this->m_free_heap_indices[this->m_free_heap_indices.size() - 1];
		this->m_free_heap_indices.pop_back();

		m_mutex.unlock();
		return index;
	}

	void SubHeapManager::Free(uint32_t index)
	{
		m_mutex.lock();

		this->m_free_heap_indices.push_back(index);
		this->free_index_count++;

		m_mutex.unlock();
	}

}

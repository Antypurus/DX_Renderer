#pragma once
#include <mutex>
#include <vector>
#include <atomic>

namespace DXR
{

	struct HeapManager
	{

	};

	struct SubHeapManager
	{
	public:
		std::atomic<uint32_t> free_index_count;
	private:
		std::mutex m_mutex;
		std::vector<uint32_t> m_free_heap_indices;
	public:
		SubHeapManager(const std::vector<uint32_t>& HeapIndices);
		uint32_t Allocate();
	};

}

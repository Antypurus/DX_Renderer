#pragma once
#include <mutex>
#include <vector>
#include <atomic>
#include <unordered_map>
#include "DescriptorHeap.hpp"

namespace DXR
{

	using IndexToSubManagerMap = std::unordered_map<uint32_t,size_t>;

	struct SubHeapManager
	{
	public:
		std::atomic<uint32_t> free_index_count = 0;
	private:
		std::unordered_map<uint32_t,bool> allocated_indices;
		std::vector<uint32_t> m_free_heap_indices;
		std::mutex m_mutex;
	public:
		SubHeapManager() = default;
		SubHeapManager(const std::vector<uint32_t>& HeapIndices);
		uint32_t Allocate();
		void Free(uint32_t index);
	};

	struct HeapManager
	{
	public:
		std::atomic<UINT64> next_manager = 0;

		UINT heap_size;
		UINT submanager_count;

		DescriptorHeap descriptor_heap;
		DescriptorType heap_type;

		std::vector<SubHeapManager> managers;
		IndexToSubManagerMap index_mapping;

	public:
		HeapManager(UINT HeapSize, UINT SubManagerCount);
		uint32_t Allocate();
		void Free(uint32_t index);
	};

}

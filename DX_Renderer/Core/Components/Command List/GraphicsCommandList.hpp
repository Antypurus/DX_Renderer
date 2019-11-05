#pragma once

#include <d3d12.h>
#include <wrl.h>
#include "../GraphicsDevice.hpp"

namespace DXR
{
	using namespace Microsoft;

	class GraphicsCommandList
	{
	public:
	private:
		WRL::ComPtr<ID3D12CommandAllocator> m_command_allocator;
		WRL::ComPtr<ID3D12GraphicsCommandList> m_command_list;
	public:
		GraphicsCommandList(GraphicsDevice& device);
		ID3D12GraphicsCommandList* operator->();
	private:
		inline void CreateCommandAllocator(GraphicsDevice& device);
		inline void CreateCommandList(GraphicsDevice& device);
	};
}

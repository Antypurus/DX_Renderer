#pragma once

#include <d3d12.h>
#include <wrl.h>
#include "../GraphicsDevice.hpp"

namespace DXR
{
	using namespace Microsoft;

	struct GraphicsCommandList
	{
	public:
		friend GraphicsDevice;
	private:
		WRL::ComPtr<ID3D12CommandAllocator> m_command_allocator;
		WRL::ComPtr<ID3D12GraphicsCommandList> m_command_list;
	public:
		ID3D12GraphicsCommandList* operator->();
		ID3D12GraphicsCommandList* GetRAWInterface() const;
		ID3D12CommandAllocator* GetCommandAllocator();
	private:
		GraphicsCommandList(GraphicsDevice& device);
		inline void CreateCommandAllocator(GraphicsDevice& device);
		inline void CreateCommandList(GraphicsDevice& device);
	};
}

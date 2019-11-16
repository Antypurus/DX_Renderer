#pragma once

#include <d3d12.h>
#include <wrl.h>

namespace DXR
{
	using namespace Microsoft;

	struct GraphicsDevice;

	struct Fence
	{
	public:
		friend GraphicsDevice;
		UINT64 initial_value = 0;
	private:
		UINT64 current_value = 0;
		WRL::ComPtr<ID3D12Fence> m_fence;
	public:
		Fence(UINT64 initialValue,GraphicsDevice& device);
	private:
	};
}
#include "Fence.hpp"
#include "GraphicsDevice.hpp"
#include "../../Tooling/Validate.hpp"

namespace DXR
{
	Fence::Fence(UINT64 initialValue, GraphicsDevice& device)
	{
		this->initial_value = initialValue;
		this->current_value = current_value;
		INFO_LOG(L"Creting Fence\n");
		DXCall(device->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->m_fence)));
		SUCCESS_LOG(L"Fence Created\n");
	}
}

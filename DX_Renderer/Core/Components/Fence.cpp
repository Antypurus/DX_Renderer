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

	void Fence::Advance()
	{
		this->current_value++;
		INFO_LOG(L"Fence Value Advaned\n");
	}

	void Fence::Signal(CommandQueue& queue) const
	{
		INFO_LOG(L"Attempting To Signal Fence\n");
		DXCall(queue->Signal(this->m_fence.Get(),this->current_value));
		SUCCESS_LOG(L"Fence Signaled\n");
	}

	UINT64 Fence::GetCompletedValue() const
	{
		return this->m_fence->GetCompletedValue();
	}

	void Fence::WaitForFence() const
	{
		INFO_LOG(L"Started Waring On Fence\n");
		if(this->GetCompletedValue()<this->current_value)
		{
			HANDLE eventHandle = CreateEventEx(nullptr,false,false,EVENT_ALL_ACCESS);
			this->m_fence->SetEventOnCompletion(current_value,eventHandle);
			WaitForSingleObject(eventHandle,INFINITE);
			CloseHandle(eventHandle);
		}
		SUCCESS_LOG(L"Finished Waiting On Fence\n");
	}
}

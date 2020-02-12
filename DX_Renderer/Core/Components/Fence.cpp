#include "Fence.hpp"
#include "GraphicsDevice.hpp"
#include "../../Tooling/Validate.hpp"

namespace DXR
{
	Fence::Fence(UINT64 initialValue, GraphicsDevice& device)
	{
		this->initial_value = initialValue;
		this->current_value = current_value;
		INFO_LOG(L"Creting Fence");
		DXCall(device->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->m_fence)));
		this->CreateWaitEvent();
		SUCCESS_LOG(L"Fence Created");
	}

	Fence::~Fence()
	{
		if(this->m_wait_event_handle != nullptr)
		{
			CloseHandle(this->m_wait_event_handle);
		}
	}

	void Fence::Advance()
	{
		this->current_value++;
	}

	void Fence::Signal(CommandQueue& queue) const
	{
		DXCall(queue->Signal(this->m_fence.Get(),this->current_value));
	}

	UINT64 Fence::GetCompletedValue() const
	{
		return this->m_fence->GetCompletedValue();
	}

	void Fence::WaitForFence() const
	{
		while(this->GetCompletedValue()<this->current_value)
		{
			this->m_fence->SetEventOnCompletion(current_value, this->m_wait_event_handle);
			WaitForSingleObject(this->m_wait_event_handle,INFINITE);
		}
	}

	void Fence::CreateWaitEvent()
	{
		this->m_wait_event_handle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if(this->m_wait_event_handle == nullptr)
		{
		#ifndef NDEBUG
			DWORD error = GetLastError();
			wchar_t* msg = FormatErrorMessage(error);
			DXR::LogError(__FILENAME__, __LINE__, msg);
			free(msg);
			__debugbreak();
		#else
			exit(-1);
		#endif
		}
	}
}

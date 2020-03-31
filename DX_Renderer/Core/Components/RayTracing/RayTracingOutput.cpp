#include "RayTracingOutput.hpp"
#include "../GraphicsDevice.hpp"
#include "../Command List/GraphicsCommandList.hpp"
#include "../Swapchain.hpp"
#include "../Resource/GPU Buffers/GPUDefaultBuffer.hpp"
#include "../Resource/ResourceBarrier.hpp"

namespace DXR
{
	RayTracingOutput::RayTracingOutput(GraphicsDevice& Device, GraphicsCommandList& CommandList, Swapchain& swapchain)
	{
		this->m_texture_format = swapchain.m_backbuffer_format;
		this->m_height = swapchain.GetBackbufferResolution().Height;
		this->m_width = swapchain.GetBackbufferResolution().Width;
		this->m_heap = Device.CreateShaderResourceDescriptorHeap(1);

		this->CreateResourceDescription();
		this->CreateTextureBuffer(Device,CommandList);
		this->CreateUAVDescriptor(Device);
		this->m_texture_buffer->GetResource()->SetName(L"RT Output Buffer");
	}

	//TODO(Tiago): Variables Need To Be Cleaned Up A Bit
	void RayTracingOutput::CopyToBackbuffer(GraphicsCommandList& CommandList, Swapchain& swapchain)
	{
		TransitionResourceBarrier barrier1(*swapchain.GetCurrentBackbufferResource(),D3D12_RESOURCE_STATE_RENDER_TARGET,D3D12_RESOURCE_STATE_COPY_DEST);
		TransitionResourceBarrier barrier2(*this->m_texture_buffer->GetResource(),D3D12_RESOURCE_STATE_UNORDERED_ACCESS,D3D12_RESOURCE_STATE_COPY_SOURCE);

		barrier1.ExecuteResourceBarrier(CommandList);
		barrier2.ExecuteResourceBarrier(CommandList);

		CommandList->CopyResource(swapchain.GetCurrentBackbufferResource(),this->m_texture_buffer->GetResource());

		TransitionResourceBarrier barrier3(*swapchain.GetCurrentBackbufferResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);
		TransitionResourceBarrier barrier4(*this->m_texture_buffer->GetResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		barrier3.ExecuteResourceBarrier(CommandList);
		barrier4.ExecuteResourceBarrier(CommandList);
	}

	DescriptorHeap* RayTracingOutput::GetDescriptorHeap()
	{
		return &this->m_heap;
	}

	D3D12_GPU_VIRTUAL_ADDRESS RayTracingOutput::GetGPUAddress()
	{
		return this->m_texture_buffer->GetGPUAddress();
	}

	void RayTracingOutput::CreateResourceDescription()
	{
		m_texture_desc.Format = this->m_texture_format;
		m_texture_desc.Alignment = 0;
		m_texture_desc.DepthOrArraySize = 1;
		m_texture_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		m_texture_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		m_texture_desc.Height = this->m_height;
		m_texture_desc.Width = this->m_width;
		m_texture_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		m_texture_desc.MipLevels = 1;
		m_texture_desc.SampleDesc = {1,0};
	}

	void RayTracingOutput::CreateShaderResourceViewDescription(GraphicsDevice& Device) const
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		srv_desc.Format = this->m_texture_format;
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D = {};
		srv_desc.Texture2D.MipLevels = 1;
		srv_desc.Texture2D.MostDetailedMip = 0;
		srv_desc.Texture2D.ResourceMinLODClamp = 0;
		srv_desc.Texture2D.PlaneSlice = 0;

		//Device->CreateShaderResourceView(this->m_texture_buffer->GetResource(), &srv_desc, this->m_heap[0]);
	}

	void RayTracingOutput::CreateTextureBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{
		this->m_texture_buffer = std::make_unique<GPUDefaultBuffer>(Device,CommandList,1,1,this->m_texture_desc,D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}

	void RayTracingOutput::CreateUAVDescriptor(GraphicsDevice& Device)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uav = {};
		uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		Device->CreateUnorderedAccessView(this->m_texture_buffer->GetResource(),nullptr,&uav,this->m_heap[0]);
	}

}
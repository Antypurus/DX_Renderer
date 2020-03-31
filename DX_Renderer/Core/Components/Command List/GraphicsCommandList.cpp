#include "GraphicsCommandList.hpp"
#include "../../../Tooling/Validate.hpp"
#include "../Pipeline/PipelineStateObject.hpp"
#include "../Resource/RenderTargetView.hpp"
#include "../Resource/DepthStencilBuffer.hpp"
#include "../Vertices/IndexBuffer.hpp"
#include <vector>
#include "../Resource/Texture/Texture.hpp"
#include "../RayTracing/AccelerationStructure.hpp"

namespace DXR
{
	GraphicsCommandList::GraphicsCommandList(GraphicsDevice& device)
	{
		this->CreateCommandAllocator(device);
		this->CreateCommandList(device);
	}

	ID3D12GraphicsCommandList4* GraphicsCommandList::operator->() const
	{
		return this->m_command_list.Get();
	}

	ID3D12GraphicsCommandList4* GraphicsCommandList::GetRAWInterface() const
	{
		return this->m_command_list.Get();
	}

	ID3D12CommandAllocator* GraphicsCommandList::GetCommandAllocator() const
	{
		return this->m_command_allocator.Get();
	}

	void GraphicsCommandList::ResetCommandAllocator() const
	{
		DXCall(this->m_command_allocator->Reset());
	}

	void GraphicsCommandList::ResetCommandList(PipelineStateObject& pso) const
	{
		DXCall(this->m_command_list->Reset(this->m_command_allocator.Get(),pso.GetPipelineStateObject()));
	}

	void GraphicsCommandList::FullReset(PipelineStateObject& pso) const
	{
		this->ResetCommandAllocator();
		this->ResetCommandList(pso);
	}

	void GraphicsCommandList::SetName(const std::wstring& CommandListName) const
	{
		this->m_command_list->SetName(CommandListName.c_str());
	}

	void GraphicsCommandList::SetGraphicsRootSignature(const RootSignature& RootSignature) const
	{
		this->m_command_list->SetGraphicsRootSignature(RootSignature.GetRootSignature());
	}

	void GraphicsCommandList::SetDisplayRenderTarget(const RenderTargetView& RenderTarget,
		const DepthStencilBuffer& ZBuffer)
	{
		this->m_command_list->OMSetRenderTargets(1, &RenderTarget.GetCPUHandle(), FALSE, &ZBuffer.GetCPUHandle());
	}

	void GraphicsCommandList::BindIndexBuffer(IndexBuffer& IndexBuffer)
	{
		this->m_current_index_buffer = &IndexBuffer;
		this->m_command_list->IASetIndexBuffer(&IndexBuffer.GetIndexBufferDescriptor());
	}

	void GraphicsCommandList::BindDescriptorHeap(const DescriptorHeap& DescriptorHeap)
	{
		ID3D12DescriptorHeap* Heap[1] = { DescriptorHeap.GetRAWInterface() };
		this->m_command_list->SetDescriptorHeaps(1, Heap);
	}

	void GraphicsCommandList::BindDescriptorHeaps(const std::vector<DescriptorHeap*>& DescriptorHeaps)
	{
		ID3D12DescriptorHeap** Heaps = new ID3D12DescriptorHeap* [DescriptorHeaps.size()];
		for (size_t heap_index = 0; heap_index < DescriptorHeaps.size(); ++heap_index)
		{
			Heaps[heap_index] = DescriptorHeaps[heap_index]->GetRAWInterface();
		}
		this->m_command_list->SetDescriptorHeaps(DescriptorHeaps.size(), Heaps);
		delete[] Heaps;
	}

	void GraphicsCommandList::SetPrimitiveTopology(PrimitiveTopology Topology)
	{
		this->m_current_primitive_topology = Topology;
		switch (m_current_primitive_topology)
		{
		case DXR::PrimitiveTopology::None:
		{
			break;
		}
		case DXR::PrimitiveTopology::Points:
		{
			this->m_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
			break;
		}
		case DXR::PrimitiveTopology::Lines:
		{
			this->m_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
			break;
		}
		case DXR::PrimitiveTopology::Triangles:
		{
			this->m_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			break;
		}
		default:
			break;
		}
	}

	void GraphicsCommandList::SendDrawCall()
	{
		if (this->m_current_index_buffer == nullptr) return;
		if (this->m_current_primitive_topology == PrimitiveTopology::None)
		{
			this->SetPrimitiveTopology(this->DefaultPrimitiveTopology);
		}

		this->m_command_list->DrawIndexedInstanced(this->m_current_index_buffer->GetIndexCount(), 1, 0, 0, 0);
	}

	void GraphicsCommandList::Close() const
	{
		DXCall(this->m_command_list->Close());
		this->m_current_primitive_topology = PrimitiveTopology::None;
	}

	void GraphicsCommandList::BindTexture(Texture& texture, UINT slot)
	{
		this->m_command_list->SetGraphicsRootDescriptorTable(3,texture.m_sampler.GetGPUHandle());
		this->m_command_list->SetGraphicsRootDescriptorTable(2,texture.GetGPUHandle());
	}

	void GraphicsCommandList::BindTLAS(TLAS& tlas, UINT slot)
	{
		this->m_command_list->SetComputeRootShaderResourceView(slot,tlas.GetTLASGPUAddress());
	}

	inline void GraphicsCommandList::CreateCommandAllocator(GraphicsDevice& device)
	{
		INFO_LOG(L"Creating Command Allocator");
		DXCall(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->m_command_allocator)));
		SUCCESS_LOG(L"Command Allocator Created");
	}

	inline void GraphicsCommandList::CreateCommandList(GraphicsDevice& device)
	{
		INFO_LOG(L"Creating Graphics Command List");
		DXCall(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, this->m_command_allocator.Get(),nullptr,IID_PPV_ARGS(&this->m_command_list)));
		SUCCESS_LOG(L"Graphics Command List Created");
		DXCall(this->m_command_list->Close());
	}
}
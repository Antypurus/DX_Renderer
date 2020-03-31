#pragma once

#include <d3d12.h>
#include <wrl.h>
#include "../GraphicsDevice.hpp"
#include <string>


#include "../Resource/GPU Buffers/ConstantBuffer.hpp"
#include "../Vertices/VertexBuffer.hpp"

namespace DXR
{
	struct Texture;
	struct IndexBuffer;
	struct DepthStencilBuffer;
	struct RenderTargetView;
	struct PipelineStateObject;
	struct RootSignature;
	struct TLAS;
	template<typename VertexStruct> struct VertexBuffer;
	using namespace Microsoft;

	enum class PrimitiveTopology
	{
		None,
		Points,
		Lines,
		Triangles
	};

	struct GraphicsCommandList
	{
	public:
		const static PrimitiveTopology DefaultPrimitiveTopology = PrimitiveTopology::Triangles;
		friend GraphicsDevice;
	private:
		WRL::ComPtr<ID3D12CommandAllocator> m_command_allocator;
		WRL::ComPtr<ID3D12GraphicsCommandList4> m_command_list;
		IndexBuffer* m_current_index_buffer = nullptr;
		mutable PrimitiveTopology m_current_primitive_topology = PrimitiveTopology::None;
	public:
		ID3D12GraphicsCommandList4* operator->() const;
		[[nodiscard]] ID3D12GraphicsCommandList4* GetRAWInterface() const;
		[[nodiscard]] ID3D12CommandAllocator* GetCommandAllocator() const;
		void ResetCommandAllocator() const;
		void ResetCommandList(PipelineStateObject& pso) const;
		void FullReset(PipelineStateObject& pso) const;
		void SetName(const std::wstring& CommandListName) const;
		void SetGraphicsRootSignature(const RootSignature& RootSignature) const;
		void SetDisplayRenderTarget(const RenderTargetView& RenderTarget, const DepthStencilBuffer& ZBuffer);
		void BindIndexBuffer(IndexBuffer& IndexBuffer);
		void BindDescriptorHeap(const DescriptorHeap& DescriptorHeap);
		void BindDescriptorHeaps(const std::vector<DescriptorHeap*>& DescriptorHeaps);
		void SetPrimitiveTopology(PrimitiveTopology Topology);
		void SendDrawCall();
		void Close() const;
		void BindTexture(Texture& texture, UINT slot);
		void BindTLAS(TLAS& tlas, UINT slot);

		template<typename T>
		void BindVertexBuffer(VertexBuffer<T>& VertexBuffer);

		template<typename T>
		void BindConstantBuffer(ConstantBuffer<T>& ConstantBuffer, UINT Slot);
	private:
		GraphicsCommandList(GraphicsDevice& device);
		inline void CreateCommandAllocator(GraphicsDevice& device);
		inline void CreateCommandList(GraphicsDevice& device);
	};

	template <typename T>
	void GraphicsCommandList::BindVertexBuffer(VertexBuffer<T>& VertexBuffer)
	{
		this->m_command_list->IASetVertexBuffers(0, 1, &VertexBuffer.GetVertexBufferDescriptor());
	}

	template <typename T>
	void GraphicsCommandList::BindConstantBuffer(ConstantBuffer<T>& ConstantBuffer, UINT Slot)
	{
		//this->m_command_list->SetGraphicsRootDescriptorTable(Slot, ConstantBuffer.GetGPUHandle());
		this->m_command_list->SetGraphicsRootConstantBufferView(Slot, ConstantBuffer.m_resource->GetGPUVirtualAddress());
	}
}

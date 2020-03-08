#pragma once

#include <d3d12.h>
#include <wrl.h>
#include "../GraphicsDevice.hpp"
#include <string>

#include "../Vertices/VertexBuffer.hpp"

namespace DXR
{
	struct IndexBuffer;
	struct DepthStencilBuffer;
	struct RenderTargetView;
	struct PipelineStateObject;
	struct RootSignature;
	using namespace Microsoft;

	struct GraphicsCommandList
	{
	public:
		friend GraphicsDevice;
	private:
		WRL::ComPtr<ID3D12CommandAllocator> m_command_allocator;
		WRL::ComPtr<ID3D12GraphicsCommandList> m_command_list;
		IndexBuffer* m_current_index_buffer = nullptr;
	public:
		ID3D12GraphicsCommandList* operator->() const;
		[[nodiscard]] ID3D12GraphicsCommandList* GetRAWInterface() const;
		[[nodiscard]] ID3D12CommandAllocator* GetCommandAllocator() const;
		void ResetCommandAllocator() const;
		void ResetCommandList(PipelineStateObject& pso) const;
		void FullReset(PipelineStateObject& pso) const;
		void SetName(const std::wstring& CommandListName) const;
		void SetGraphicsRootSignature(const RootSignature& RootSignature) const;
		void SetDisplayRenderTarget(const RenderTargetView& RenderTarget, const DepthStencilBuffer& ZBuffer);
		void BindIndexBuffer(IndexBuffer& IndexBuffer);

		template<typename T>
		void BindVertexBuffer(VertexBuffer<T>& VertexBuffer);
	private:
		GraphicsCommandList(GraphicsDevice& device);
		inline void CreateCommandAllocator(GraphicsDevice& device);
		inline void CreateCommandList(GraphicsDevice& device);
	};

	template <typename T>
	void GraphicsCommandList::BindVertexBuffer(VertexBuffer<T>& VertexBuffer)
	{
		this->m_command_list->IASetVertexBuffers(0,1,&VertexBuffer.GetVertexBufferDescriptor());
	}
}

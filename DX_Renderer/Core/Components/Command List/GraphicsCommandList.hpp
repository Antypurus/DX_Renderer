#pragma once

#include <d3d12.h>
#include <wrl.h>
#include "../GraphicsDevice.hpp"
#include <string>

namespace DXR
{
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
	private:
		GraphicsCommandList(GraphicsDevice& device);
		inline void CreateCommandAllocator(GraphicsDevice& device);
		inline void CreateCommandList(GraphicsDevice& device);
	};
}

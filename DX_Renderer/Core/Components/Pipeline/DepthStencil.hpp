#pragma once

#include <d3d12.h>

namespace DXR
{
    
	struct StencilOperation
	{
        public:
        private:
		D3D12_STENCIL_OP m_stencil_fail_op = D3D12_STENCIL_OP_KEEP;
		D3D12_STENCIL_OP m_stencil_depth_fail_op = D3D12_STENCIL_OP_KEEP;
		D3D12_STENCIL_OP m_stencil_pass_op = D3D12_STENCIL_OP_KEEP;
		D3D12_COMPARISON_FUNC m_stencil_function = D3D12_COMPARISON_FUNC_ALWAYS;
        public:
		static StencilOperation Default();
		D3D12_DEPTH_STENCILOP_DESC GetStencilOperationDescription();
        private:
	};
    
	struct DepthStencil
	{
        public:
        private:
		BOOL m_depth_enabled = FALSE;
		D3D12_DEPTH_WRITE_MASK m_depth_write_mask = D3D12_DEPTH_WRITE_MASK_ALL;
		D3D12_COMPARISON_FUNC m_depth_function = D3D12_COMPARISON_FUNC_LESS;
		BOOL m_stencil_enabled = TRUE;
		UINT8 m_stencil_read_mask = D3D12_DEFAULT_STENCIL_READ_MASK;
		UINT8 m_stencil_write_mask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		D3D12_DEPTH_STENCILOP_DESC m_front_face = StencilOperation::Default().GetStencilOperationDescription();
		D3D12_DEPTH_STENCILOP_DESC m_back_face = StencilOperation::Default().GetStencilOperationDescription();
        public:
		static DepthStencil Default();
		D3D12_DEPTH_STENCIL_DESC GetDepthStencilDescription();
        private:
	};
    
}
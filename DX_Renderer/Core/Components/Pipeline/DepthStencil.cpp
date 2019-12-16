#include "DepthStencil.hpp"

namespace DXR
{
	StencilOperation StencilOperation::Default()
	{
		return StencilOperation();
	}

	D3D12_DEPTH_STENCILOP_DESC StencilOperation::GetStencilOperationDescription()
	{
		D3D12_DEPTH_STENCILOP_DESC stencil_op = {};
		stencil_op.StencilDepthFailOp = this->m_stencil_depth_fail_op;
		stencil_op.StencilFailOp = this->m_stencil_fail_op;
		stencil_op.StencilFunc = this->m_stencil_function;
		stencil_op.StencilPassOp = this->m_stencil_pass_op;

		return stencil_op;
	}

	DepthStencil DepthStencil::Default()
	{
		return DepthStencil();
	}
	
	D3D12_DEPTH_STENCIL_DESC DepthStencil::GetDepthStencilDescription()
	{
		D3D12_DEPTH_STENCIL_DESC depth_stencil = {};
		depth_stencil.BackFace = this->m_back_face;
		depth_stencil.FrontFace = this->m_front_face;
		depth_stencil.DepthEnable = this->m_depth_enabled;
		depth_stencil.DepthWriteMask = this->m_depth_write_mask;
		depth_stencil.DepthFunc = this->m_depth_function;
		depth_stencil.StencilEnable = this->m_stencil_enabled;
		depth_stencil.StencilReadMask = this->m_stencil_read_mask;
		depth_stencil.StencilWriteMask = this->m_stencil_write_mask;

		return depth_stencil;
	}
}
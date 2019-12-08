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
}
#include "Blend.hpp"

namespace DXR
{
	RenderTargetBlend RenderTargetBlend::Default()
	{
		return RenderTargetBlend();
	}

	D3D12_RENDER_TARGET_BLEND_DESC RenderTargetBlend::GetBlendDescription()
	{
		D3D12_RENDER_TARGET_BLEND_DESC render_target_blend = {};
		render_target_blend.BlendEnable = this->m_blend_enabled;
		render_target_blend.LogicOpEnable = this->m_logical_operation_enabled;

		render_target_blend.SrcBlend = this->m_source_blend;
		render_target_blend.DestBlend = this->m_destination_blend;
		render_target_blend.BlendOp = this->m_blend_operation;

		render_target_blend.SrcBlendAlpha = this->m_source_blend_alpha;
		render_target_blend.DestBlendAlpha = this->m_destination_blend_alpha;
		render_target_blend.BlendOpAlpha = this->m_blend_alpha_operation;

		render_target_blend.LogicOp = this->m_logic_operation;
		render_target_blend.RenderTargetWriteMask = this->m_render_target_write_mask;

		return render_target_blend;
	}

	Blend Blend::Default()
	{
		return Blend();
	}

	D3D12_BLEND_DESC Blend::GetBlendDescription()
	{
		D3D12_BLEND_DESC blend = {};
		blend.AlphaToCoverageEnable = this->m_alpha_to_coverage_enabled;
		blend.IndependentBlendEnable = this->m_independent_blend_enabled;

		D3D12_RENDER_TARGET_BLEND_DESC renter_taget_blend = RenderTargetBlend::Default().GetBlendDescription();
		for(size_t i = 0;i < 8;++i)
		{
			blend.RenderTarget[i] = renter_taget_blend;
		}

		return blend;
	}
}
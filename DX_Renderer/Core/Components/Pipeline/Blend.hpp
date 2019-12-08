#pragma once

#include <d3d12.h>

namespace DXR
{

	struct RenderTargetBlend
	{
	public:
	private:
		BOOL m_blend_enabled = FALSE;
		BOOL m_logical_operation_enabled = FALSE;

		D3D12_BLEND m_source_blend = D3D12_BLEND_ONE;
		D3D12_BLEND m_destination_blend = D3D12_BLEND_ZERO;
		D3D12_BLEND_OP m_blend_operation = D3D12_BLEND_OP_ADD;

		D3D12_BLEND m_source_blend_alpha = D3D12_BLEND_ONE;
		D3D12_BLEND m_destination_blend_alpha = D3D12_BLEND_ZERO;
		D3D12_BLEND_OP m_blend_alpha_operation = D3D12_BLEND_OP_ADD;

		D3D12_LOGIC_OP m_logic_operation = D3D12_LOGIC_OP_NOOP;
		UINT8 m_render_target_write_mask = D3D12_COLOR_WRITE_ENABLE_ALL;
	public:
		static RenderTargetBlend Default();
		D3D12_RENDER_TARGET_BLEND_DESC GetBlendDescription();
	private:
	};

	struct Blend
	{
	public:
	private:
		BOOL m_alpha_to_coverage_enabled = FALSE;
		BOOL m_independent_blend_enabled = FALSE;
		RenderTargetBlend m_render_target_blend = RenderTargetBlend::Default();
	public:
		static Blend Default();
		D3D12_BLEND_DESC GetBlendDescription();
	private:
	};
}
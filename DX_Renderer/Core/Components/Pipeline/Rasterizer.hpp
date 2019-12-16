#pragma once

#include <d3d12.h>

namespace DXR
{
	struct Rasterizer
	{
	public:
	private:
		D3D12_FILL_MODE m_fill_mode = D3D12_FILL_MODE_WIREFRAME;
		D3D12_CULL_MODE m_cull_mode = D3D12_CULL_MODE_NONE;
		D3D12_CONSERVATIVE_RASTERIZATION_MODE m_conservative_rasterization = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		INT m_depth_bias = D3D12_DEFAULT_DEPTH_BIAS;
		FLOAT m_depth_bias_clamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		FLOAT m_slope_scaled_depth_bias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		BOOL m_depth_clip_enabled = TRUE;
		BOOL m_multisample_enabled = FALSE;
		BOOL m_antialiased_lines_enabled = FALSE;
		UINT m_forced_sample_count = 0;
		BOOL m_front_counter_clockwise = FALSE;
	public:
		static Rasterizer Default();
		D3D12_RASTERIZER_DESC GetRasterizerDescription();
	private:
	};
}
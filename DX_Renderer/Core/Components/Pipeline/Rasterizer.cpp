#include "Rasterizer.hpp"

namespace DXR
{
	Rasterizer Rasterizer::Default()
	{
		return Rasterizer();
	}

	D3D12_RASTERIZER_DESC Rasterizer::GetRasterizerDescription()
	{
		D3D12_RASTERIZER_DESC rasterizer = {};
		rasterizer.AntialiasedLineEnable = this->m_antialiased_lines_enabled;
		rasterizer.ConservativeRaster = this->m_conservative_rasterization;
		rasterizer.FillMode = this->m_fill_mode;
		rasterizer.CullMode = this->m_cull_mode;
		rasterizer.DepthBias = this->m_depth_bias;
		rasterizer.DepthBiasClamp = this->m_depth_bias_clamp;
		rasterizer.SlopeScaledDepthBias = this->m_slope_scaled_depth_bias;
		rasterizer.DepthClipEnable = this->m_depth_clip_enabled;
		rasterizer.ForcedSampleCount = this->m_forced_sample_count;
		rasterizer.FrontCounterClockwise = this->m_front_counter_clockwise;
		rasterizer.MultisampleEnable = this->m_multisample_enabled;

		return rasterizer;
	}
}
#pragma once

#include <wrl.h>
#include <d3d12.h>
#include "Rasterizer.hpp"
#include "../Shader/Root Signature/RootSignature.hpp"
#include "Blend.hpp"
#include "DepthStencil.hpp"

namespace DXR
{
	using namespace Microsoft;

	struct PipelineStateObject
	{
	public:
	private:
		WRL::ComPtr<ID3D12PipelineState> m_pso;

		// information
		RootSignature m_root_signature; //passed in
		D3D12_STREAM_OUTPUT_DESC m_stream_output = {0};
		Blend m_blend = Blend::Default();
		Rasterizer m_rasterizer = Rasterizer::Default();
		DepthStencil m_depth_stencil = DepthStencil::Default();
		UINT m_sample_mask = UINT_MAX;
		D3D12_INPUT_LAYOUT_DESC m_input_layout; // passed in
		D3D12_INDEX_BUFFER_STRIP_CUT_VALUE m_index_buffer_strip_cut_value = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE m_primitive_type = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		UINT m_render_target_count = 1;
		DXGI_FORMAT m_backbuffer_format; // passed in
		DXGI_FORMAT m_depth_stencil_buffer_format; // passed in
		DXGI_SAMPLE_DESC m_mssa = {1,0};
		UINT m_node_mask = 0;
		D3D12_CACHED_PIPELINE_STATE m_cached_pso = {0};
		D3D12_PIPELINE_STATE_FLAGS m_pso_flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		// shaders
		D3D12_SHADER_BYTECODE m_vertex_shader = {0};
		D3D12_SHADER_BYTECODE m_hull_shader = {0};
		D3D12_SHADER_BYTECODE m_domain_shader = {0};
		D3D12_SHADER_BYTECODE m_geometry_shader = {0};
		D3D12_SHADER_BYTECODE m_pixel_shader = {0};
	public:

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC CreatePipelineStateObjectDescription();
	};

}
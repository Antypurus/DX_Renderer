#include "PipelineStateObject.hpp"
#include "../GraphicsDevice.hpp"
#include "../../../Tooling/Validate.hpp"
#include "d3dx12.h"

namespace DXR
{
	PipelineStateObject::PipelineStateObject(GraphicsDevice& device, D3D12_SHADER_BYTECODE& vertexShader, D3D12_SHADER_BYTECODE& pixelShader, RootSignature& rootSignature, D3D12_INPUT_LAYOUT_DESC& inputLayout, DXGI_FORMAT backbufferFormat, DXGI_FORMAT dsvFormat)
	{
		this->m_vertex_shader = vertexShader;
		this->m_pixel_shader = pixelShader;
		this->m_root_signature = rootSignature;
		this->m_input_layout = inputLayout;
		this->m_backbuffer_format = backbufferFormat;
		this->m_depth_stencil_buffer_format = dsvFormat;

		this->CreatePipelineStateObject(device);
	}

	ID3D12PipelineState* PipelineStateObject::GetPipelineStateObject()
	{
		return this->m_pso.Get();
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateObject::CreatePipelineStateObjectDescription()
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
		pso.pRootSignature = this->m_root_signature.GetRootSignature();
		pso.BlendState = this->m_blend.GetBlendDescription();
		pso.RasterizerState = this->m_rasterizer.GetRasterizerDescription();
		pso.PrimitiveTopologyType = this->m_primitive_type;
		pso.CachedPSO = this->m_cached_pso;
		pso.DepthStencilState = this->m_depth_stencil.GetDepthStencilDescription();
		pso.DSVFormat = this->m_depth_stencil_buffer_format;
		pso.Flags = this->m_pso_flags;
		pso.IBStripCutValue = this->m_index_buffer_strip_cut_value;
		pso.InputLayout = this->m_input_layout;
		pso.NodeMask = this->m_node_mask;
		pso.NumRenderTargets = 1;//this->m_render_target_count;
		pso.RTVFormats[0] = this->m_backbuffer_format;
		pso.SampleDesc = this->m_mssa;
		pso.SampleMask = UINT_MAX;
		pso.StreamOutput = this->m_stream_output;

		pso.VS = this->m_vertex_shader;
		pso.HS = this->m_hull_shader;
		pso.DS = this->m_domain_shader;
		pso.GS = this->m_geometry_shader;
		pso.PS = this->m_pixel_shader;

		return pso;
	}

	void PipelineStateObject::CreatePipelineStateObject(GraphicsDevice& device)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = this->CreatePipelineStateObjectDescription();
		DXCall(device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&this->m_pso)));
		delete[] pso_desc.InputLayout.pInputElementDescs;
	}
}
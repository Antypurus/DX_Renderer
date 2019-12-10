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
		pso.InputLayout = this->m_input_layout;
		pso.pRootSignature = this->m_root_signature.GetRootSignature();
		pso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pso.DepthStencilState = {};
		pso.SampleMask = UINT_MAX;
		pso.PrimitiveTopologyType = this->m_primitive_type;
		//pso.DSVFormat = this->m_depth_stencil_buffer_format;
		pso.NumRenderTargets = 1;
		pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pso.SampleDesc.Count = 1;

		pso.VS = this->m_vertex_shader;
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
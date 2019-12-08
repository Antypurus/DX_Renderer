#include "PipelineStateObject.hpp"

namespace DXR
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateObject::CreatePipelineStateObjectDescription()
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
		pso.BlendState = this->m_blend;
		pso.CachedPSO = this->m_cached_pso;
		pso.DepthStencilState = this->m_depth_stencil.GetDepthStencilDescription();
		pso.DSVFormat = this->m_depth_stencil_buffer_format;
		pso.Flags = this->m_pso_flags;
		pso.IBStripCutValue = this->m_index_buffer_strip_cut_value;
		pso.InputLayout = this->m_input_layout;
		pso.NodeMask = this->m_node_mask;
		pso.NumRenderTargets = this->m_render_target_count;
		pso.PrimitiveTopologyType = this->m_primitive_type;
		pso.pRootSignature = this->m_root_signature.GetRootSignature();

		pso.VS = this->m_vertex_shader;
		pso.HS = this->m_hull_shader;
		pso.DS = this->m_domain_shader;
		pso.GS = this->m_geometry_shader;
		pso.PS = this->m_pixel_shader;
		
		return pso;
	}
}
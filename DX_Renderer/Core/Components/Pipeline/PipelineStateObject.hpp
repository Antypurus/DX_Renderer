#pragma once

#include <wrl.h>
#include <d3d12.h>
#include "Rasterizer.hpp"
#include "../Shader/Root Signature/RootSignature.hpp"
#include "Blend.hpp"

namespace DXR
{
	using namespace Microsoft;

	struct PipelineStateObject
	{
	public:
	private:
		RootSignature m_root_signature;
		Rasterizer m_rasterizer = Rasterizer::Default();
		Blend m_blend = Blend::Default();
	public:
	private:
	};

}
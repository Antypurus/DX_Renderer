#pragma once

#include <wrl.h>
#include <d3d12.h>
#include "Rasterizer.hpp"
#include "../Shader/Root Signature/RootSignature.hpp"

namespace DXR
{
	using namespace Microsoft;

	struct PipelineStateObject
	{
	public:
	private:
		Rasterizer m_rasterizer;
		RootSignature m_root_signature;
	public:
	private:
	};

}
#pragma once

#include <d3d12.h>
#include <wrl.h>

namespace DXR
{

	using namespace Microsoft;
	
	struct RootSignature
	{
	public:
	private:
		WRL::ComPtr<ID3D12RootSignature> m_root_signature;
	public:
	private:
	};
}
#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include "ConstantRootParameter.hpp"
#include "DescriptorTableRootParameter.hpp"
#include "DescriptorRootParameter.hpp"

namespace DXR
{
	struct GraphicsDevice;

	using namespace Microsoft;

	//TODO(Tiago): Support Static Samplers
	struct RootSignature
	{
	public:
	private:
		WRL::ComPtr<ID3D12RootSignature> m_root_signature;
		std::vector<ConstantRootParameter> m_constant_root_parameters;
		std::vector<DescriptorTableRootParameter> m_descriptor_table_root_parameters;
		std::vector<DescriptorRootParameter> m_descriptor_root_parameters;
	public:
		RootSignature();
		void AddConstantRootParameter(ConstantRootParameter& rootParameter);
		void AddDescriptorRootParameter(DescriptorRootParameter& rootParameter);
		void AddDescriptorTableRootParameter(DescriptorTableRootParameter& rootParameter);
		void CreateRootSignature(GraphicsDevice& device);
		
	private:
	};
}

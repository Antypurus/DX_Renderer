#pragma once
#include "RootParameter.hpp"
#include <vector>

namespace DXR
{

	struct DescriptorTableRootParameter : public RootParameter
	{
	public:
	private:
		std::vector<D3D12_DESCRIPTOR_RANGE> m_table_entries;
	public:
		DescriptorTableRootParameter(D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void AddSRVEntry(UINT DescriptorCount);
		void AddUAVEntry(UINT DescriptorCount);
		void AddCBVEntry(UINT DescriptorCount);
		void AddSamplerEntry(UINT DescriptorCount);
	protected:
		D3D12_ROOT_DESCRIPTOR_TABLE GenerateDescriptorTable();
		D3D12_ROOT_PARAMETER GenerateRootParameter() override;
	};

}

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
		DescriptorTableRootParameter();
		void AddSRVEntry(UINT DescriptorCount);
		void AddUAVEntry(UINT DescriptorCount);
		void AddCBVEntry(UINT DescriptorCount);
		void AddSamplerEntry(UINT DescriptorCount);
	protected:
		D3D12_ROOT_PARAMETER GenerateRootParameter() override;
	};

}

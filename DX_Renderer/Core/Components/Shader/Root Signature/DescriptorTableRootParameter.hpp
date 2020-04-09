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
		void AddSRVEntry(UINT Register,UINT Space = 0, UINT DescriptorCount = 1);
		void AddUAVEntry(UINT Register,UINT Space = 0, UINT DescriptorCount = 1);
		void AddCBVEntry(UINT Register,UINT Space = 0, UINT DescriptorCount = 1);
		void AddSamplerEntry(UINT Register,UINT Space = 0, UINT DescriptorCount = 1);
	protected:
		D3D12_ROOT_DESCRIPTOR_TABLE GenerateDescriptorTable();
		D3D12_ROOT_PARAMETER GenerateRootParameter() override;
	};

}

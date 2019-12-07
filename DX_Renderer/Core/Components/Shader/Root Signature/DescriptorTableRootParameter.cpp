#include "DescriptorTableRootParameter.hpp"

namespace DXR
{
	DescriptorTableRootParameter::DescriptorTableRootParameter() :RootParameter(RootParameterType::DescriptorTable)
	{
	}

	void DescriptorTableRootParameter::AddSRVEntry(UINT DescriptorCount)
	{
		D3D12_DESCRIPTOR_RANGE table_entry = {};
		table_entry.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		table_entry.BaseShaderRegister = 0;
		table_entry.NumDescriptors = DescriptorCount;
		table_entry.RegisterSpace = 0;
		table_entry.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		this->m_table_entries.emplace_back(table_entry);
	}

	void DescriptorTableRootParameter::AddUAVEntry(UINT DescriptorCount)
	{
		D3D12_DESCRIPTOR_RANGE table_entry = {};
		table_entry.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		table_entry.BaseShaderRegister = 0;
		table_entry.NumDescriptors = DescriptorCount;
		table_entry.RegisterSpace = 0;
		table_entry.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		this->m_table_entries.emplace_back(table_entry);
	}

	void DescriptorTableRootParameter::AddCBVEntry(UINT DescriptorCount)
	{
		D3D12_DESCRIPTOR_RANGE table_entry = {};
		table_entry.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		table_entry.BaseShaderRegister = 0;
		table_entry.NumDescriptors = DescriptorCount;
		table_entry.RegisterSpace = 0;
		table_entry.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		this->m_table_entries.emplace_back(table_entry);
	}

	void DescriptorTableRootParameter::AddSamplerEntry(UINT DescriptorCount)
	{
		D3D12_DESCRIPTOR_RANGE table_entry = {};
		table_entry.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		table_entry.BaseShaderRegister = 0;
		table_entry.NumDescriptors = DescriptorCount;
		table_entry.RegisterSpace = 0;
		table_entry.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		this->m_table_entries.emplace_back(table_entry);
	}

	D3D12_ROOT_PARAMETER DescriptorTableRootParameter::GenerateRootParameter()
	{
	}
}

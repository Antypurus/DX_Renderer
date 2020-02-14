#include "DescriptorTableRootParameter.hpp"

namespace DXR
{
	DescriptorTableRootParameter::DescriptorTableRootParameter(D3D12_SHADER_VISIBILITY visibility)
		:RootParameter(RootParameterType::DescriptorTable, visibility)
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

	D3D12_ROOT_DESCRIPTOR_TABLE DescriptorTableRootParameter::GenerateDescriptorTable()
	{
		const uint64_t entry_count = this->m_table_entries.size();
		D3D12_DESCRIPTOR_RANGE* descriptor_table_entries = new D3D12_DESCRIPTOR_RANGE[entry_count];
		for(size_t i = 0;i < entry_count;++i)
		{
			descriptor_table_entries[i] = this->m_table_entries[i];
		}

		D3D12_ROOT_DESCRIPTOR_TABLE descriptor_table = {};
		descriptor_table.NumDescriptorRanges = this->m_table_entries.size();
		descriptor_table.pDescriptorRanges = descriptor_table_entries;

		return descriptor_table;
	}

	D3D12_ROOT_PARAMETER DescriptorTableRootParameter::GenerateRootParameter()
	{
		D3D12_ROOT_PARAMETER root_parameter = {};
		root_parameter.DescriptorTable = this->GenerateDescriptorTable();
		root_parameter.ParameterType = this->m_parameter_type;
		root_parameter.ShaderVisibility = this->m_shader_visibility;

		return root_parameter;
	}
}

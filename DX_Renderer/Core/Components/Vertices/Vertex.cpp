#include "Vertex.hpp"

namespace DXR
{
	Vertex::Vertex(XMFLOAT3 position/*, XMFLOAT4 color*/)
	{
		this->m_position = position;
		//this->m_color = color;
	}

	const XMFLOAT3& Vertex::GetPosition() const
	{
		return this->m_position;
	}

	std::vector<D3D12_INPUT_ELEMENT_DESC> Vertex::GenerateInputElementDescription()
	{
		D3D12_INPUT_ELEMENT_DESC position_element_description = {};
		position_element_description.SemanticName = "POSITION";
		position_element_description.SemanticIndex = 0;
		position_element_description.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		position_element_description.InputSlot = 0;
		position_element_description.AlignedByteOffset = 0;
		position_element_description.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		position_element_description.InstanceDataStepRate = 0;

		//D3D12_INPUT_ELEMENT_DESC color_element_description = {};
		//position_element_description.SemanticName = "COLOR";
		//position_element_description.SemanticIndex = 0;
		//position_element_description.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		//position_element_description.InputSlot = 0;
		//position_element_description.AlignedByteOffset = 12;
		//position_element_description.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		//position_element_description.InstanceDataStepRate = 0;

		std::vector<D3D12_INPUT_ELEMENT_DESC> input_elements_descriptions = {position_element_description/*,color_element_description*/};
		return input_elements_descriptions;
	}

	UINT64 Vertex::GetElementSize() const
	{
		return sizeof(m_position);
	}

	void Vertex::operator=(Vertex other)
	{
		other.m_position = this->m_position;
	}
	void* Vertex::GetData()
	{
		return this;
	}

	D3D12_INPUT_LAYOUT_DESC Vertex::GetInputLayout()
	{
		D3D12_INPUT_ELEMENT_DESC position_element_description = {};
		position_element_description.SemanticName = "POSITION";
		position_element_description.SemanticIndex = 0;
		position_element_description.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		position_element_description.InputSlot = 0;
		position_element_description.AlignedByteOffset = 0;
		position_element_description.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		position_element_description.InstanceDataStepRate = 0;

		D3D12_INPUT_ELEMENT_DESC* elements = new  D3D12_INPUT_ELEMENT_DESC[1];
		elements[0] = position_element_description;
		D3D12_INPUT_LAYOUT_DESC input_layout = {};
		input_layout.NumElements = 1;
		input_layout.pInputElementDescs = elements;

		return input_layout;
	}
}

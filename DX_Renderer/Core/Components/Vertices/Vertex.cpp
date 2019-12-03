#include "Vertex.hpp"

namespace DXR
{
	Vertex::Vertex(Coordinates coordinates)
	{
		this->m_coordinates = coordinates;
	}

	const Coordinates& Vertex::GetCoordinates()
	{
		return this->m_coordinates;
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

		std::vector<D3D12_INPUT_ELEMENT_DESC> input_elements_descriptions = {position_element_description};
		return input_elements_descriptions;
	}
}

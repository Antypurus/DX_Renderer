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

	D3D12_INPUT_ELEMENT_DESC Vertex::GenerateInputElementDescription()
	{
		D3D12_INPUT_ELEMENT_DESC element_description = {};
		element_description.SemanticName = "POSITION";
		element_description.SemanticIndex = 0;
		element_description.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		element_description.InputSlot = 0;
		element_description.AlignedByteOffset = 0;
		element_description.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		element_description.InstanceDataStepRate = 0;
		return element_description;
	}
}

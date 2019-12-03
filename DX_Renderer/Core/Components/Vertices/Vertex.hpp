#pragma once

#include<d3d12.h>
#include<vector>

namespace DXR
{

	typedef long long int int64;

	struct Coordinates
	{
		int64 x_coord;
		int64 y_coord;
		int64 z_coord;
	};

	struct Vertex
	{
	public:
	protected:
		Coordinates m_coordinates;
	public:
		Vertex(Coordinates coordinates);
		const Coordinates& GetCoordinates();
		virtual std::vector<D3D12_INPUT_ELEMENT_DESC> GenerateInputElementDescription();
	protected:
	};

}

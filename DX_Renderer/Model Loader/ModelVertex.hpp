#pragma once
#include "../Core/Components/Vertices/Vertex.hpp"

namespace DXR
{

	struct OBJVertex :public Vertex
	{
	public:
		XMFLOAT3 position;
		XMFLOAT2 uv;
		XMFLOAT3 normal;
		XMFLOAT3 color;
	public:
		OBJVertex() = default;
		OBJVertex(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT2& uv, const XMFLOAT3& color);
		std::vector<D3D12_INPUT_ELEMENT_DESC> GenerateInputElementDescription() override;
		UINT64 GetElementSize() const override;
		void* GetData() override;
		void operator=(Vertex other) override;
		static D3D12_INPUT_LAYOUT_DESC GetInputLayout();
	};

}

#pragma once

#include<d3d12.h>
#include<vector>
#include<DirectXMath.h>

namespace DXR
{

	using namespace DirectX;
	
	struct Vertex
	{
	public:
	protected:
		XMFLOAT3 m_position = {0,0,0};
	public:
		Vertex() = default;
		Vertex(XMFLOAT3 position);
		const XMFLOAT3& GetPosition() const;
		virtual std::vector<D3D12_INPUT_ELEMENT_DESC> GenerateInputElementDescription();
		virtual UINT64 GetElementSize() const;
		virtual void operator=(Vertex other);
	protected:
	};

}

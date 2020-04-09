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
		XMFLOAT3 m_position = {};
		XMFLOAT2 m_uv = {};
	public:
		Vertex() = default;
		Vertex(XMFLOAT3 position,XMFLOAT2 UV);
		const XMFLOAT3& GetPosition() const;
		virtual std::vector<D3D12_INPUT_ELEMENT_DESC> GenerateInputElementDescription();
		virtual UINT64 GetElementSize() const;
		virtual void* GetData();
		virtual void operator=(Vertex other);
		static D3D12_INPUT_LAYOUT_DESC GetInputLayout();
	protected:
	};

}

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
        bool operator==(const OBJVertex& other) const;
		static D3D12_INPUT_LAYOUT_DESC GetInputLayout();
        XMFLOAT3 GetPosition() const;
	};
    
}

namespace std
{
    template<> class hash<DXR::OBJVertex>
    {
    public:
        size_t operator()(const DXR::OBJVertex& vertex) const
        {
            //TODO(Tiago): This hashing function is just a placeholder and need to be improved, it onl takes position into accoutn and even then its still not a very good hashing function i dont think
            return
                (((UINT)(vertex.GetPosition().x) ^ ((UINT)(vertex.GetPosition().y) << 1)) >> 1)
                ^ ((UINT)(vertex.GetPosition().z) << 1);
        }
    };
}
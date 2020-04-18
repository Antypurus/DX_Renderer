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
		void operator=(const OBJVertex& other);
        bool operator==(const OBJVertex& other) const;
		static D3D12_INPUT_LAYOUT_DESC GetInputLayout();
        XMFLOAT3 GetPosition() const;
	};
    
}

void hash_combine_vertices(size_t& seed, size_t hash);

namespace std
{
    template<> class hash<DXR::OBJVertex>
    {
    public:
        size_t operator()(const DXR::OBJVertex& vertex) const
        {
            //TODO(Tiago): This is stone from glm and only accounts for the position so it can likely be improved
			size_t seed = 0;
			hash_combine_vertices(seed,static_cast<size_t>(vertex.position.x));
			hash_combine_vertices(seed,static_cast<size_t>(vertex.position.y));
			hash_combine_vertices(seed,static_cast<size_t>(vertex.position.z));
			return seed;
        }
    };
}
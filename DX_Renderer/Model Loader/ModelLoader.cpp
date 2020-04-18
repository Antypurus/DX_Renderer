#define TINYOBJLOADER_IMPLEMENTATION

#include "ModelLoader.hpp"
#include "../ThirdParty/tiny_obj_loader/tiny_obj_loader.h"
#include "../Core/Components/GraphicsDevice.hpp"
#include "../Core/Components/Command List/GraphicsCommandList.hpp"
#include "../Core/Components/Vertices/IndexBuffer.hpp"

#include <unordered_map>

namespace DXR
{
    //TODO(Tiago): Loading of the chatedral insides seem a bit off, but the overall architecture also seems to be right so it might be an issue with the model itself, need to investigate this further and test more models
	OBJMesh OBJModelLoader::Load(const std::string& filepath)
	{
        
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
        
		std::string warn;
		std::string err;
        
		bool res = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filepath.c_str(), 0, true);
		if (!res)
		{
			//TODO(Tiago): Handle load fail
			return OBJMesh();
		}
        
		if(!err.empty())
		{
			//TODO(Tiago): Handle this error
		}
        
        std::unordered_map<UINT,bool> index_map;
        
		std::vector<OBJVertex> vertices;
		std::vector<UINT> indices;
        
		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
                XMFLOAT3 pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };
                
                XMFLOAT2 uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
                };
                
                XMFLOAT3 normal = {
                    0,0,0
                };
                
                XMFLOAT3 color = {0.0f,0.0f,0.0f};
                
                vertices.push_back({pos,normal,uv,color});
                indices.push_back(indices.size());
			}
		}
        
		return OBJMesh(vertices, indices);
	}
    
	OBJMesh::OBJMesh(const std::vector<OBJVertex>& vertices, const std::vector<UINT>& indices)
	{
		this->vertices = vertices;
		this->indices = indices;
	}
    
	VertexBuffer<OBJVertex> OBJMesh::GenerateVertexBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{
		return VertexBuffer<OBJVertex>(Device, CommandList, this->vertices);
	}
    
	IndexBuffer OBJMesh::GenerateIndexBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{
		return IndexBuffer(Device, CommandList, this->indices);
	}
}
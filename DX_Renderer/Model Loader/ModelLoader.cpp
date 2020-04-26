#define TINYOBJLOADER_IMPLEMENTATION

#include "ModelLoader.hpp"
#include "../ThirdParty/tiny_obj_loader/tiny_obj_loader.h"
#include "../Core/Components/GraphicsDevice.hpp"
#include "../Core/Components/Command List/GraphicsCommandList.hpp"
#include "../Core/Components/Vertices/IndexBuffer.hpp"

#include <unordered_map>
#include <algorithm>

namespace DXR
{
	OBJMesh OBJModelLoader::Load(const std::string& filepath)
	{

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn;
		std::string err;

		bool res = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filepath.c_str(), 0);
		if (!res)
		{
			//TODO(Tiago): Handle load fail
			return OBJMesh();
		}

		if (!err.empty())
		{
			//TODO(Tiago): Handle this error
		}

		std::unordered_map<OBJVertex, UINT> vertex_map;

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

				XMFLOAT3 normal;
				if (attrib.normals.size() > 0)
				{
					normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}
				else
				{
					normal = {
						0,0,0
					};
				}

				XMFLOAT3 color = { 0.0f,0.0f,0.0f };

				OBJVertex vert = { pos,normal,uv,color };

				if (vertex_map.count(vert) == 0)
				{
					vertex_map[vert] = static_cast<UINT>(vertices.size());
					vertices.push_back(vert);
				}
				indices.push_back(vertex_map[vert]);
			}
		}

		return OBJMesh(vertices, indices);
	}

	OBJMesh::OBJMesh(const std::vector<OBJVertex>& vertices, const std::vector<UINT>& indices)
	{
		for (const auto& vertex : vertices)
		{
			this->vertices.push_back(vertex);
		}
		for (const auto& index : indices)
		{
			this->indices.push_back(index);
		}
		this->DetermineAABB();
	}

	VertexBuffer<OBJVertex> OBJMesh::GenerateVertexBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{
		return VertexBuffer<OBJVertex>(Device, CommandList, this->vertices);
	}

	IndexBuffer OBJMesh::GenerateIndexBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{
		return IndexBuffer(Device, CommandList, this->indices);
	}

	void OBJMesh::DetermineAABB()
	{
		using namespace std;
		AABB[0] = AABB[1] = vertices[0].position;
		for (std::vector<OBJVertex>::iterator it = vertices.begin(); it != vertices.end(); ++it) 
		{
			AABB[0].x = min(AABB[0].x, it->position.x);
			AABB[0].y = min(AABB[0].y, it->position.y);
			AABB[0].z = min(AABB[0].z, it->position.z);
			AABB[1].x = max(AABB[1].x, it->position.x);
			AABB[1].y = max(AABB[1].y, it->position.y);
			AABB[1].z = max(AABB[1].z, it->position.z);
		}
	}
}
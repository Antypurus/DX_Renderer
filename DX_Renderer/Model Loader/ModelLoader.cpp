#define TINYOBJLOADER_IMPLEMENTATION

#include "ModelLoader.hpp"
#include "../ThirdParty/tiny_obj_loader/tiny_obj_loader.h"
#include "../Core/Components/GraphicsDevice.hpp"
#include "../Core/Components/Command List/GraphicsCommandList.hpp"
#include "../Core/Components/Vertices/IndexBuffer.hpp"

namespace DXR
{
	OBJMesh OBJModelLoader::Load(const std::string& filepath)
	{

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn;
		std::string err;

		bool res = tinyobj::LoadObj(&attrib,&shapes,&materials,&warn,&err,filepath.c_str(),0,true);
		if(!res || !err.empty())
		{
			//LOG LOAD FAIL
			return OBJMesh();
		}

		if(!warn.empty())
		{
			//LOG Warning
		}

		std::vector<OBJVertex> vertices;
		std::vector<UINT> indices;

		for(size_t vert = 0;vert<attrib.vertices.size()/3;++vert)
		{
			float pos_x = attrib.vertices[vert];
			float pos_y = attrib.vertices[vert + 1];
			float pos_z = attrib.vertices[vert + 2];

			float normal_x = 0;
			float normal_y = 0;
			float normal_z = 0;

			float tex_u = attrib.texcoords[vert];
			float tex_v = attrib.texcoords[vert + 1];

			float color_r = 0;
			float color_g = 0;
			float color_b = 0;

			vertices.push_back({{pos_x,pos_y,pos_z},{normal_x,normal_y,normal_z},{tex_u,tex_v},{color_r,color_g,color_b}});
		}

		// Loop over shapes and get indices
		for (size_t s = 0; s < shapes.size(); s++) {

			for(size_t v = 0;v  < shapes[s].mesh.indices.size(); ++v)
			{
				tinyobj::index_t idx = shapes[s].mesh.indices[v];
				indices.push_back(idx.vertex_index);
			}

		}

		return OBJMesh(vertices,indices);
	}

	OBJMesh::OBJMesh(const std::vector<OBJVertex>& vertices, const std::vector<UINT>& indices)
	{
		this->vertices = vertices;
		this->indices = indices;
	}

	VertexBuffer<OBJVertex> OBJMesh::GenerateVertexBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{
		return VertexBuffer<OBJVertex>(Device,CommandList,this->vertices);
	}

	IndexBuffer OBJMesh::GenerateIndexBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{
		return IndexBuffer(Device,CommandList,this->indices);
	}
}
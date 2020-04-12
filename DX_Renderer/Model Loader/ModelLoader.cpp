#define TINYOBJLOADER_IMPLEMENTATION

#include "ModelLoader.hpp"
#include "../ThirdParty/tiny_obj_loader/tiny_obj_loader.h"

namespace DXR
{
	OBJMesh OBJModelLoader::Load(const std::string& filepath, GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn;
		std::string err;

		bool res = tinyobj::LoadObj(&attrib,&shapes,&materials,&warn,&err,filepath.c_str(),0,true);
		if(!res || !warn.empty() || !err.empty())
		{
			//LOG LOAD FAIL
			return OBJMesh();
		}

		std::vector<OBJVertex> vertices;
		std::vector<UINT> indices;

		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				int fv = shapes[s].mesh.num_face_vertices[f];

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					tinyobj::real_t vertex_x = attrib.vertices[3 * idx.vertex_index + 0];
					tinyobj::real_t vertex_y = attrib.vertices[3 * idx.vertex_index + 1];
					tinyobj::real_t vertex_z = attrib.vertices[3 * idx.vertex_index + 2];

					tinyobj::real_t normal_x = attrib.normals[3 * idx.normal_index + 0];
					tinyobj::real_t normal_y = attrib.normals[3 * idx.normal_index + 1];
					tinyobj::real_t normal_z = attrib.normals[3 * idx.normal_index + 2];
					
					tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
					
					tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
					tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
					tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];

					OBJVertex vertex = {{vertex_x,vertex_y,vertex_z},{normal_x,normal_y,normal_z},{tx,ty},{red,green,blue}};
					vertices.push_back(vertex);
				}
				index_offset += fv;

				// per-face material
				shapes[s].mesh.material_ids[f];
			}

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
}
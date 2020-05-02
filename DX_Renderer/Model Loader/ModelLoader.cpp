#define TINYOBJLOADER_IMPLEMENTATION

#include "ModelLoader.hpp"
#include "../ThirdParty/tiny_obj_loader/tiny_obj_loader.h"
#include "../Core/Components/GraphicsDevice.hpp"
#include "../Core/Components/Command List/GraphicsCommandList.hpp"
#include "../Core/Components/Vertices/IndexBuffer.hpp"

#include <unordered_map>
#include <algorithm>
#include <locale>
#include <codecvt>

namespace DXR
{
	Model ModelLoader::LoadOBJ(const std::string& filepath, GraphicsDevice& device, GraphicsCommandList& command_list)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
        
		std::string warn;
		std::string err;
        
		std::string folder = ModelLoader::DetermineFolder(filepath);
        
		bool res = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str(), folder.c_str());
		if (!res)
		{
			//TODO(Tiago): Handle load fail
			return Model();
		}
        
		if (!err.empty())
		{
			//TODO(Tiago): Handle this error
		}
        
		std::vector<Material> model_materials;
		//Generate Material Data
		{
			for (auto& material : materials)
            {
                //TODO(Tiago): i really dont like this sytem where we have to proccess the string to fix the stupid design of tinyobj loader, in the future I should just make my own obj loader that isnt shit
				size_t pos = 0;
				while ((pos = material.diffuse_texname.find("\\", pos)) != std::string::npos) {
					material.diffuse_texname.replace(pos, std::string("\\").length(), "/");
					pos += std::string("/").length();
				}
				std::string diffuse_texture_path = folder + material.diffuse_texname;
                
				Material model_material;
				model_material.name = material.name;
				model_material.ambient_coefficient = { 0,0,0 };
				model_material.diffuse_coefficient = { 0,0,0 };
				model_material.specular_coefficient = { 0,0,0 };
				if (diffuse_texture_path != folder)
				{
                    //NOTE(Tiago): if the texture is not kept alive, the system freaks out cuz it will be deleted at the end of the scope, but that will happen while the command list is open and that will create a massive issue, maybe in the future i need a system to schedule resources for deletion
					model_material.has_texture = true;
					model_material.texture = std::make_shared<Texture>(std::wstring(diffuse_texture_path.begin(), diffuse_texture_path.end()), device, command_list);
				}
                model_materials.push_back(model_material);
			}
		}
        
		std::unordered_map<OBJVertex, UINT> vertex_map;
		std::unordered_map<UINT, std::vector<UINT>> submeshes;//NOTE(Tiago): Could be substituted by an array of arrays maybe
        
		std::vector<OBJVertex> vertices;
		std::vector<UINT> indices;
        
		for (const auto& shape : shapes)
		{
			for (size_t i = 0; i < shape.mesh.indices.size() / 3; ++i)
			{
				UINT face_indices[3];
				for (unsigned int j = 0; j < 3; j++)
				{
					auto index = shape.mesh.indices[i * 3 + j];
                    
					XMFLOAT3 pos = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};
                    
					XMFLOAT2 uv;
					if (attrib.texcoords.size() > 0)
					{
						uv = {
							attrib.texcoords[2 * index.texcoord_index + 0],
							1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
						};
					}
                    
					XMFLOAT3 normal;
					if (attrib.normals.size() > 0)
					{
						normal = {
							attrib.normals[3 * index.normal_index + 0],
							attrib.normals[3 * index.normal_index + 1],
							attrib.normals[3 * index.normal_index + 2]
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
					face_indices[j] = vertex_map[vert];
				}
				auto material_id = shape.mesh.material_ids[i];
				submeshes[material_id].push_back(face_indices[0]);
				submeshes[material_id].push_back(face_indices[1]);
				submeshes[material_id].push_back(face_indices[2]);
			}
		}
        
		return Model(vertices, indices, submeshes, model_materials);
	}
    
	std::string ModelLoader::DetermineFolder(const std::string& filepath)
	{
		size_t pos = filepath.find_last_of("/\\");
		return (std::string::npos == pos) ? "" : (filepath.substr(0, pos) + "/");
	}
    
	Material::Material(const Material& other)
	{
		this->name = other.name;
		this->ambient_coefficient = other.ambient_coefficient;
		this->diffuse_coefficient = other.diffuse_coefficient;
		this->specular_coefficient = other.specular_coefficient;
		this->has_texture = other.has_texture;
		this->texture = other.texture;
	}
    
    Submesh::Submesh(const std::vector<UINT>& indices, Material& material, GraphicsDevice& device, GraphicsCommandList& command_list)
    {
        this->indices = indices;
        this->material = &material;
        this->index_buffer = std::make_unique<IndexBuffer>(device, command_list, this->indices);
    }
    
    void Submesh::Draw(GraphicsCommandList& command_list, UINT texture_slot, UINT sampler_slot)
    {
        command_list.BindIndexBuffer(*this->index_buffer);
        if(this->material->has_texture)
        {
            command_list.BindTexture(*this->material->texture, texture_slot, sampler_slot);
        }
        command_list.SendDrawCall();
    }
    
	Model::Model(const std::vector<OBJVertex>& vertices,
                 const std::vector<UINT>& indices,
                 const std::unordered_map<UINT, std::vector<UINT>>& submeshes,
                 std::vector<Material>& materials)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->materials = materials;
		this->DetermineAABB();
	}
    
	VertexBuffer<OBJVertex> Model::GenerateVertexBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{
		return VertexBuffer<OBJVertex>(Device, CommandList, this->vertices);
	}
    
	IndexBuffer Model::GenerateIndexBuffer(GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{
		return IndexBuffer(Device, CommandList, this->indices);
	}
    
	void Model::DetermineAABB()
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
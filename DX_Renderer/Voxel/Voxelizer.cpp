#include "Voxelizer.hpp"
#include "../Core/Components/GraphicsDevice.hpp"
#include "../Core/Components/Command List/GraphicsCommandList.hpp"
#include "../Camera/Camera.hpp"
#include "../Model Loader/ModelLoader.hpp"
#include "../Core/Components/Swapchain.hpp"

#include <string>

namespace DXR
{
    
    Voxelizer::Voxelizer(GraphicsDevice& device,
                         GraphicsCommandList& command_list,
                         RootSignature& root_signature,
                         Model& model,
                         XMMATRIX mvp)
    {
        this->CreateVoxelizationShaders();
        this->voxel_map = VoxelMap(device,VOXEL_WIDTH,VOXEL_HEIGHT,VOXEL_DEPTH);
        this->model = &model;
        this->pso = PipelineStateObject(device,
                                        this->voxelization_vertex_shader.GetShaderBytecode(),
                                        this->voxelization_pixel_shader.GetShaderBytecode(),
                                        root_signature,
                                        OBJVertex::GetInputLayout(),
                                        Swapchain::m_backbuffer_format,
                                        DepthStencilBuffer::DepthStencilBufferFormat);
        //NOTE(Tiago): dummy/placeholder data is required to create the constant buffer while the matrices are not yet computed
        std::vector<Voxelization_CBuffer> intermediate;
        intermediate.push_back({XMMatrixIdentity(), XMMatrixIdentity()});
        this->voxelization_cbuffer = std::make_unique<ConstantBuffer<Voxelization_CBuffer>>(device,intermediate);
        this->CalculateVoxelizationSupportData();
    }
    
    void Voxelizer::CalculateVoxelizationSupportData()
    {
        using namespace std;
        
        XMFLOAT3* AABB = model->AABB;
		XMVECTOR extent_vector = XMVectorSubtract({ AABB[1].x,AABB[1].y,AABB[1].z,1 }, { AABB[0].x,AABB[0].y,AABB[0].z,1 });
		XMStoreFloat3(&extent, extent_vector);
		extent.x *= float(VOXEL_WIDTH + 2.0f) / float(VOXEL_WIDTH);
		extent.y *= float(VOXEL_HEIGHT + 2.0f) / float(VOXEL_HEIGHT);
		extent.z *= float(VOXEL_DEPTH + 2.0f) / float(VOXEL_DEPTH);
		//extent.x = extent.y = extent.z = max(extent.x, max(extent.y, extent.z));
        
		XMVECTOR center_vector = XMVectorAdd({ AABB[1].x,AABB[1].y,AABB[1].z,1 }, { AABB[0].x,AABB[0].y,AABB[0].z,1 });
		center_vector = XMVectorScale(center_vector, 0.5f);
		XMStoreFloat3(&center, center_vector);
        
		XMVECTOR voxel_space_vector = XMVectorScale({ extent.x,extent.y,extent.z,1 }, 0.5f);
		voxel_space_vector = XMVectorSubtract(center_vector, voxel_space_vector);
		XMStoreFloat3(&voxel_space, voxel_space_vector);
    }
    
    void Voxelizer::UpdateVoxelizationMatrices(Camera& camera, XMMATRIX& model_matrix)
    {
        XMMATRIX voxel_matrix;
        XMMATRIX m1,m2;
        m1 = XMMatrixTranslation(-voxel_space.x, -voxel_space.y, -voxel_space.z);
		m2 = XMMatrixScaling(VOXEL_WIDTH / extent.x, VOXEL_HEIGHT / extent.y, VOXEL_DEPTH / extent.z);
        voxel_matrix = m1 * m2;
        voxel_space_conversion_matrix = model_matrix * camera.ViewMatrix() * voxel_matrix;
        
        XMMATRIX clip_matrix;
        m1 = XMMatrixTranslation(-center.x, -center.y, -voxel_space.z);
		m2 = XMMatrixScaling(2.0f / extent.x, 2.0f / extent.y, 1.0f / extent.z);
        clip_matrix = m1 * m2;
        clip_space_conversion_matrix = model_matrix * camera.ViewMatrix() * clip_matrix;
    }
    
    void Voxelizer::UpdateVoxelizationCBuffer()
    {
        voxelization_cbuffer->UpdateData({{
                                                 clip_space_conversion_matrix,
                                                 voxel_space_conversion_matrix
                                             }});
    }
    
    void Voxelizer::CreateVoxelizationShaders()
    {
        const std::wstring voxel_shader_path = L"./DX_Renderer/Resources/Shaders/Voxelization.hlsl";
        const std::wstring VS_entrypoint = L"VoxelVSMain";
        const std::wstring PS_entrypoint = L"VoxelPSMain";
        this->voxelization_vertex_shader = VertexShader::CompileShaderFromFile(voxel_shader_path, VS_entrypoint);
        this->voxelization_pixel_shader = PixelShader::CompileShaderFromFile(voxel_shader_path, PS_entrypoint);
    }
    
}

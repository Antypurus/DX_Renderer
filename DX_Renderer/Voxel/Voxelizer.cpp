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

#include "Voxelizer.hpp"
#include "../Core/Components/GraphicsDevice.hpp"
#include "../Core/Components/Command List/GraphicsCommandList.hpp"
#include "../Camera/Camera.hpp"
#include "../Model Loader/ModelLoader.hpp"

#include <string>

namespace DXR
{
    
    Voxelizer::Voxelizer(GraphicsDevice& device,
                         GraphicsCommandList& command_list,
                         Swapchain& swapchain,
                         RootSignature& root_signature,
                         Model& model,
                         XMMATRIX mvp)
    {
        this->CreateVoxelizationShaders();
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

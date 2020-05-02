#pragma once

#include "../Core/Components/Shader/PixelShader.hpp"
#include "../Core/Components/Shader/VertexShader.hpp"

namespace DXR
{
    
    struct GraphicsDevice;
    struct GraphicsCommandList;
    
    struct Voxelizer
    {
        VertexShader voxelization_vertex_shader;
        PixelShader voxelization_pixel_shader;
        
        Voxelizer() = default;
        Voxelizer(GraphicsDevice& device, GraphicsCommandList& command_list);
    };
    
}

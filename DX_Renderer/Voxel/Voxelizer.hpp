#pragma once

#include "../Core/Components/Shader/PixelShader.hpp"
#include "../Core/Components/Shader/VertexShader.hpp"
#include "VoxelMap.hpp"

#include <directxmath.h>

namespace DXR
{
    
    struct GraphicsDevice;
    struct GraphicsCommandList;
    struct Model;
    
    struct Voxelizer
    {
        VertexShader voxelization_vertex_shader;
        PixelShader voxelization_pixel_shader;
        VoxelMap voxel_map;
        Model* model;
        //NOTE(Tiago):voxelization matrices support data
        XMFLOAT3 extent;
        XMFLOAT3 center;
        XMFLOAT3 voxel_space;
        XMMATRIX voxel_space_conversion_matrix;
        XMMATRIX clip_space_conversion_matrix;
        
        Voxelizer() = default;
        Voxelizer(GraphicsDevice& device, GraphicsCommandList& command_list);
    };
    
}

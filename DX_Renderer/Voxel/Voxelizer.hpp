#pragma once

#include "../Core/Components/Shader/PixelShader.hpp"
#include "../Core/Components/Shader/VertexShader.hpp"
#include "../Core/Components/Pipeline/PipelineStateObject.hpp"
#include "../Core/Components/RayTracing/AccelerationStructure.hpp"
#include "../Core/Components/Resource/GPU Buffers/ConstantBuffer.hpp"
#include "VoxelMap.hpp"

#include <directxmath.h>
#include <memory>

namespace DXR
{
    
    struct Camera;
    struct GraphicsDevice;
    struct GraphicsCommandList;
    struct Model;
    struct Swapchain;
    struct RootSignature;//TODO(Tiago): Build root signature specifically for voxelization?
    
    struct Voxelization_CBuffer
    {
        XMMATRIX clip_space_transformation_matrix;
        XMMATRIX voxel_space_transformation_matrix;
    };
    
#define VOXEL_WIDTH 128
#define VOXEL_HEIGHT 128
#define VOXEL_DEPTH 128
    
    struct Voxelizer
    {
        VertexShader voxelization_vertex_shader;
        PixelShader voxelization_pixel_shader;
        VoxelMap voxel_map;
        Model* model;
        PipelineStateObject pso;
        TLAS acceleration_structure;
        std::unique_ptr<ConstantBuffer<Voxelization_CBuffer>> voxelization_cbuffer;
        //NOTE(Tiago):Voxelization matrices support data
        XMFLOAT3 extent;
        XMFLOAT3 center;
        XMFLOAT3 voxel_space;
        //NOTE(Tiago):Voxelization matrices
        XMMATRIX voxel_space_conversion_matrix;
        XMMATRIX clip_space_conversion_matrix;
        
        Voxelizer() = default;
        Voxelizer(GraphicsDevice& device,
                  GraphicsCommandList& command_list,
                  Swapchain& swapchain,
                  RootSignature& root_signature,
                  Model& model,
                  XMMATRIX mvp);
        private:
        void CalculateVoxelizationSupportData();
        void CreateVoxelizationMatrices(Camera& camera, XMMATRIX& model_matrix);
        void UpdateVoxelizationCBuffer();
        void CreateVoxelizationShaders();
    };
    
}

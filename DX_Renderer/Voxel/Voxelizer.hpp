#pragma once

#include "../Core/Components/Shader/PixelShader.hpp"
#include "../Core/Components/Shader/VertexShader.hpp"
#include "../Core/Components/Pipeline/PipelineStateObject.hpp"
#include "../Core/Components/RayTracing/AccelerationStructure.hpp"
#include "../Core/Components/Resource/GPU Buffers/ConstantBuffer.hpp"
#include "VoxelMap.hpp"
#include "../Core/Components/Vertices/VertexBuffer.hpp"
#include "../Core/Components/Vertices/IndexBuffer.hpp"
#include "../Model Loader/ModelLoader.hpp"

#include <directxmath.h>
#include <memory>

namespace DXR
{
    
    using namespace DirectX;
    
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
        VertexBuffer<OBJVertex> model_vertex_buffer;
        IndexBuffer model_index_buffer;
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
                  RootSignature& root_signature,
                  Model& model,
                  XMMATRIX mvp);
        void Voxelize(GraphicsCommandList& command_list, Camera& camera, RootSignature& root_signature, XMMATRIX model_matrix, UINT constant_buffer_slot, UINT voxel_map_uav_slot);
        private:
        void CalculateVoxelizationSupportData();
        void UpdateVoxelizationMatrices(Camera& camera, XMMATRIX& model_matrix);
        void UpdateVoxelizationCBuffer();
        void CreateVoxelizationShaders();
        void SetViewport(GraphicsCommandList& command_list);
    };
    
}

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
#include "../Core/Components/Vertices/Vertex.hpp"

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
    
#define VOXEL_WIDTH 256
#define VOXEL_HEIGHT 256
#define VOXEL_DEPTH 256
    
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
        std::unique_ptr<RootSignature> voxelization_root_signature = nullptr;
        std::unique_ptr<BLAS> voxel_cube_blas = nullptr;
        std::unique_ptr<ConstantBuffer<Voxelization_CBuffer>> voxelization_cbuffer_x = nullptr;
        std::unique_ptr<ConstantBuffer<Voxelization_CBuffer>> voxelization_cbuffer_y = nullptr;
        std::unique_ptr<ConstantBuffer<Voxelization_CBuffer>> voxelization_cbuffer_z = nullptr;
        std::unique_ptr<VertexBuffer<Vertex>> voxel_cube_vertex_buffer = nullptr;
        std::unique_ptr<IndexBuffer> voxel_cube_index_buffer = nullptr;
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
        void Voxelize(GraphicsCommandList& command_list,RootSignature& root_signature, UINT constant_buffer_slot, UINT voxel_map_uav_slot);
        void BuildAccelerationStructure(GraphicsDevice& device, GraphicsCommandList& command_list, Fence& fence);
        private:
        void CreateVoxelizationConstantBuffers(GraphicsDevice& device, GraphicsCommandList& command_list);
        void XAxisVoxelizationCall(GraphicsCommandList& command_list, UINT constant_buffer_slot);
        void YAxisVoxelizationCall(GraphicsCommandList& command_list, UINT constant_buffer_slot);
        void ZAxisVoxelizationCall(GraphicsCommandList& command_list, UINT constant_buffer_slot);
        void CalculateVoxelizationSupportData();
        void UpdateVoxelizationMatrices(XMMATRIX& model_matrix);
        void UpdateVoxelizationCBufferX();
        void UpdateVoxelizationCBufferY();
        void UpdateVoxelizationCBufferZ();
        void CreateVoxelizationShaders();
        void SetViewport(GraphicsCommandList& command_list);
        void CreateVoxelCube(GraphicsDevice& device, GraphicsCommandList& command_list);
    };
    
}

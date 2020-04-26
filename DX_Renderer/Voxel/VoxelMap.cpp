#include "VoxelMap.hpp"

#include "../Core/Components/GraphicsDevice.hpp"
#include "../Tooling/Validate.hpp"
#include "../Core/Components/Resource/HeapManager.hpp"
#include "../Core/Components/Command List/GraphicsCommandList.hpp"
#include "../Camera/Camera.hpp"
#include "../Core/Components/Resource/GPU Buffers/ConstantBuffer.hpp"
#include <vector>

namespace DXR
{
    VoxelMap::VoxelMap(GraphicsDevice& device, UINT width, UINT height, UINT depth)
    {
        this->format = DXGI_FORMAT_R32_UINT;
        this->width = width;
        this->height = height;
        this->depth = depth;
        this->CreateVoxelMap(device);
        this->CreateUAV(device);
        this->CreateVoxelConstantBuffer(device);
    }
    
    void VoxelMap::Bind(GraphicsCommandList& command_list, Camera& camera)
    {
        this->SetViewport(command_list);
        this->CreateVoxelMatrix(camera);
        this->CreateClipMatrix(camera);
        this->UpdateVoxelConstantBuffer();
    }
    
    void VoxelMap::CreateVoxelMap(GraphicsDevice& device)
    {
        D3D12_RESOURCE_DESC resource_desc;
        resource_desc.Alignment = 0;
        resource_desc.Width = this->width;
        resource_desc.Height = this->height;
        resource_desc.DepthOrArraySize = this->depth;
        resource_desc.MipLevels = 1;//TODO(Tiago): How many? I dont really know how mip mapping works on the API level
        resource_desc.SampleDesc.Count = 1;
        resource_desc.SampleDesc.Quality = 0;
        resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        resource_desc.Format = this->format;
        resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        
        D3D12_HEAP_PROPERTIES heap_properties = {};
		heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_properties.CreationNodeMask = 1;
		heap_properties.VisibleNodeMask = 1;
        
        DXCall(device->CreateCommittedResource(&heap_properties,
                                               D3D12_HEAP_FLAG_NONE,
                                               &resource_desc,
                                               D3D12_RESOURCE_STATE_COMMON,//TODO(Tiago): What is the best initial state?
                                               nullptr,
                                               IID_PPV_ARGS(&voxel_volume_texture)));
    }
    
    void VoxelMap::CreateUAV(GraphicsDevice& device)
    {
        this->heap_index = SRHeapManager::GetManager().Allocate();
        this->descriptor_heap = &SRHeapManager::GetManager().descriptor_heap;
        
        D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
        uav_desc.Format = this->format;
        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
        uav_desc.Texture3D.MipSlice = 0;
        uav_desc.Texture3D.FirstWSlice = 0;
        uav_desc.Texture3D.WSize = depth;
        
        device->CreateUnorderedAccessView(voxel_volume_texture.Get(),
                                          nullptr,
                                          &uav_desc,
                                          (*descriptor_heap)[heap_index]);
    }
    
    void VoxelMap::SetViewport(GraphicsCommandList& command_list)
    {
        D3D12_VIEWPORT viewport = {};
        viewport.Width = (FLOAT)width;
        viewport.Height = (FLOAT)height;
        viewport.MinDepth = 0;
        viewport.MaxDepth = 1;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        command_list->RSSetViewports(1,&viewport);
    }
    
    void VoxelMap::CreateVoxelMatrix(Camera& camera)
    {
        voxel_space_matrix = XMMatrixOrthographicLH((FLOAT)width,(FLOAT)height,(FLOAT)0,(FLOAT)depth) * camera.ViewMatrix();
    }
    
    void VoxelMap::CreateClipMatrix(Camera& camera)
    {
        clip_space_matrix = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, 1280.0f / 720.0f, 0.1f, 1000.0f)
            * camera.ViewMatrix();
    }
    
    void VoxelMap::CreateVoxelConstantBuffer(GraphicsDevice& device)
    {
        Voxel_cbuffer cbuffer = {};
        cbuffer.clip_space_matrix = clip_space_matrix;
        cbuffer.voxel_space_matrix = voxel_space_matrix;
        
        voxel_constant_buffer = std::make_unique<ConstantBuffer<Voxel_cbuffer>>(device,std::vector({cbuffer}));
    }
    
    void VoxelMap::UpdateVoxelConstantBuffer()
    {
        Voxel_cbuffer cbuffer = {};
        cbuffer.clip_space_matrix = clip_space_matrix;
        cbuffer.voxel_space_matrix = voxel_space_matrix;
        
        voxel_constant_buffer->UpdateData({cbuffer});
    }
    
}

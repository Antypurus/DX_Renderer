#pragma once

#include<d3d12.h>
#include<d3d12video.h>
#include<wrl.h>

namespace DXR
{
    
    using namespace Microsoft::WRL;
    
    struct GraphicsDevice;
    
    struct VideoEncodeCommandList
    {
        ComPtr<ID3D12CommandAllocator> command_allocator = nullptr;
        ComPtr<ID3D12VideoEncodeCommandList> command_list = nullptr;
        
        VideoEncodeCommandList() = default;
        VideoEncodeCommandList(GraphicsDevice& Device);
        ID3D12VideoEncodeCommandList* operator->() const;
        private:
        void CreateCommandAllocator(GraphicsDevice& Device);
        void CreateCommandList(GraphicsDevice& Device);
    };
    
}
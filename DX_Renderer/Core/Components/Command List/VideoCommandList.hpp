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
        ComPtr<ID3D12CommandAllocator> command_allocator;
        ComPtr<ID3D12VideoProcessCommandList1> command_list;
        
        VideoEncodeCommandList(GraphicsDevice& Device);
        ID3D12VideoProcessCommandList1* operator->() const;
        private:
        void CreateCommandAllocator(GraphicsDevice& Device);
        void CreateCommandList(GraphicsDevice& Device);
    };
    
}
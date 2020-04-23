#include "VideoCommandList.hpp"
#include "../GraphicsDevice.hpp"

namespace DXR
{
    VideoEncodeCommandList::VideoEncodeCommandList(GraphicsDevice& Device)
    {
        CreateCommandAllocator(Device);
        CreateCommandList(Device);
    }
    
    void VideoEncodeCommandList::CreateCommandAllocator(GraphicsDevice& Device)
    {
        DXCall(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE,
                                              IID_PPV_ARGS(&command_allocator)));
    }
    
    void VideoEncodeCommandList::CreateCommandList(GraphicsDevice& Device)
    {
        DXCall(Device->CreateCommandList(0,
                                         D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE,
                                         command_allocator.Get(),
                                         nullptr,
                                         IID_PPV_ARGS(&command_list)));
        DXCall(command_list->Close());
    }
}
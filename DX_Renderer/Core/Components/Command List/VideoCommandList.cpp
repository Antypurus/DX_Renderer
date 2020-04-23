#include "VideoCommandList.hpp"
#include "../GraphicsDevice.hpp"
#include "../../../Tooling/Validate.hpp"

namespace DXR
{
    VideoEncodeCommandList::VideoEncodeCommandList(GraphicsDevice& Device)
    {
        CreateCommandAllocator(Device);
        CreateCommandList(Device);
    }
    
    ID3D12VideoEncodeCommandList* VideoEncodeCommandList::operator->() const
    {
        return command_list.Get();
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
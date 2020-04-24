#include "VideoCommandQueue.hpp"
#include "../GraphicsDevice.hpp"
#include "../Command List/VideoCommandList.hpp"
#include "../../../Tooling/Validate.hpp"

namespace DXR
{
    
    VideoEncodeCommandQueue::VideoEncodeCommandQueue():CommandQueue(CommandQueueType::None)
    {
    }
    
    VideoEncodeCommandQueue::VideoEncodeCommandQueue(GraphicsDevice& device):CommandQueue(CommandQueueType::VideoEncode)
    {
        this->m_command_queue_type = D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE;
        this->CreateCommandQueue(device);
    }
    
    void VideoEncodeCommandQueue::ExecuteCommandList(VideoEncodeCommandList& commandList)
    {
        ID3D12CommandList* list[] = {commandList.command_list.Get()};//TODO(Tiago): should i substitute this by a function call? Prolly not fuck unnecessary getters
        this->m_command_queue->ExecuteCommandLists(1,list);
    }
    
    inline void VideoEncodeCommandQueue::CreateCommandQueue(GraphicsDevice& device)
    {
        D3D12_COMMAND_QUEUE_DESC description = {};
        description.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        description.Type = this->m_command_queue_type;
        DXCall(device->CreateCommandQueue(&description,IID_PPV_ARGS(&this->m_command_queue)));
    }
    
}

#include"VideoCommandQueue.hpp"
#include"../GraphicsDevice.hpp"
#include"../Command List/VideoCommandList.hpp"

namespace DXR
{
    VideoEncodeCommandQueue::VideoEncodeCommandQueue(GraphicsDevice& device):CommandQueue(CommandQueueType::VideoEncode)
    {
        
    }
    
    void VideoEncodeCommandQueue::ExecuteCommandList(VideoEncodeCommandList& commandList)
    {
        
    }
    
    inline void VideoEncodeCommandQueue::CreateCommandQueue(GraphicsDevice& device)
    {
        
    }
    
}

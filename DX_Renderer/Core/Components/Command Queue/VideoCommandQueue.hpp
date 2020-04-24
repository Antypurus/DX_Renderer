#pragma once

#include "CommandQueue.hpp"

namespace DXR
{
    
    struct GraphicsDevice;
    struct VideoEncodeCommandList;
    
    struct VideoEncodeCommandQueue: public CommandQueue
    {
        VideoEncodeCommandQueue(GraphicsDevice& device);
        void ExecuteCommandList(VideoEncodeCommandList& commandList);
        private:
        inline void CreateCommandQueue(GraphicsDevice& device);
    };
    
}

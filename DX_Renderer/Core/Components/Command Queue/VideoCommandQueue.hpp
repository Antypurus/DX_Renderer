#pragma once

#include "CommandQueue.hpp"

namespace DXR
{
    
    struct GraphicsDevice;
    struct VideoEncodeCommandList;
    
    struct VideoEncodeCommandQueue: public CommandQueue
    {
        VideoEncodeCommandQueue();//NOTE(Tiago):Null constructor
        VideoEncodeCommandQueue(GraphicsDevice& device);
        void ExecuteCommandList(VideoEncodeCommandList& commandList);
        private:
        inline void CreateCommandQueue(GraphicsDevice& device);
    };
    
}

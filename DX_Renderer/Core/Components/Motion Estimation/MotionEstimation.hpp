#pragma once

#include "../GraphicsDevice.hpp"
#include "../Command List/VideoCommandList.hpp"
#include "../Command Queue/VideoCommandQueue.hpp"

namespace DXR
{
    
    struct MotionEstimator
    {
        ID3D12VideoDevice1* video_device;//NOTE(Tiago): Non owning pointer
        D3D12_FEATURE_DATA_VIDEO_MOTION_ESTIMATOR motion_estimation_support = {0u, DXGI_FORMAT_NV12};
        
        WRL::ComPtr<ID3D12VideoMotionEstimator> motion_estimator;
        WRL::ComPtr<ID3D12VideoMotionVectorHeap> vector_heap;
        WRL::ComPtr<ID3D12Resource> resolved_motion_vectors;
        VideoEncodeCommandList command_list;
        VideoEncodeCommandQueue* command_queue;//TODO(Tiago): this should probably be moved to the device, so that it is easier to reuse in case its needed, that being said right now i dont see it being needed again, so lets leave it here where it is convenient
        
        MotionEstimator(GraphicsDevice& device);
        void EstimateMotion(GraphicsDevice& device, Swapchain& swapchain);
        private:
        void CreateVideoEncodingCommandList(GraphicsDevice& device);
        void CreateVideoEncodingCommandQueue(GraphicsDevice& device);
        bool QueryMotionEstimationSupport();
        void CreateMotionEstimator();
        void CreateMotionVectorHeap();
        void CreateResolvedMotionVectorTexture();
    };
    
}
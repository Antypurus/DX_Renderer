#pragma once

#include "../GraphicsDevice.hpp"
#include "../Command List/VideoCommandList.hpp"

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
        
        MotionEstimator(GraphicsDevice& device);
        void EstimateMotion(GraphicsDevice& device, Swapchain& swapchain);
        private:
        void CreateVideoEncodingCommandList(GraphicsDevice& device);
        bool QueryMotionEstimationSupport();
        void CreateMotionEstimator();
        void CreateMotionVectorHeap();
        void CreateResolvedMotionVectorTexture();
    };
    
}
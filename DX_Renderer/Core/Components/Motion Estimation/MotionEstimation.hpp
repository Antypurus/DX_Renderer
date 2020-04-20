#pragma once

#include "../GraphicsDevice.hpp"

namespace DXR
{
    
    struct MotionEstimator
    {
        ID3D12VideoDevice1* video_device;//NOTE(Tiago): Non owning pointer
        D3D12_FEATURE_DATA_VIDEO_MOTION_ESTIMATOR motion_estimation_support = {0u, DXGI_FORMAT_NV12};
        
        WRL::ComPtr<ID3D12VideoMotionEstimator> motion_estimator;
        WRL::ComPtr<ID3D12VideoMotionVectorHeap> vector_heap;
        
        MotionEstimator(GraphicsDevice& device);
        bool QueryMotionEstimationSupport();
        void CreateMotionEstimator();
    };
    
}
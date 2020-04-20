#pragma once

#include "../GraphicsDevice.hpp"

namespace DXR
{
    
    struct MotionEstimator
    {
        D3D12_FEATURE_DATA_VIDEO_MOTION_ESTIMATOR motion_estimation_support;
    };
    
}
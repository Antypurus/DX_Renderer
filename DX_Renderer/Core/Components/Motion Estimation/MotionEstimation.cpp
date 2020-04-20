#include "MotionEstimation.hpp"
#include "../../../Tooling/Validate.hpp"

namespace DXR
{
    
    MotionEstimator::MotionEstimator(GraphicsDevice& device)
    {
        this->video_device = device.GetVideoDevice();
        this->QueryMotionEstimationSupport();
        this->CreateMotionEstimator();
    }
    
    bool MotionEstimator::QueryMotionEstimationSupport()
    {
        DXCall(video_device->CheckFeatureSupport(D3D12_FEATURE_VIDEO_MOTION_ESTIMATOR, &motion_estimation_support, sizeof(motion_estimation_support)));
        return true;
    }
    
    void MotionEstimator::CreateMotionEstimator()
    {
        D3D12_VIDEO_MOTION_ESTIMATOR_DESC motionEstimatorDesc = {
            0, //NodeIndex
            DXGI_FORMAT_NV12,
            D3D12_VIDEO_MOTION_ESTIMATOR_SEARCH_BLOCK_SIZE_16X16,
            D3D12_VIDEO_MOTION_ESTIMATOR_VECTOR_PRECISION_QUARTER_PEL,
            {1920, 1080, 1280, 720} // D3D12_VIDEO_SIZE_RANGE
        };
        
        DXCall(video_device->CreateVideoMotionEstimator(
                                                        &motionEstimatorDesc,
                                                        nullptr,
                                                        IID_PPV_ARGS(&motion_estimator)));
    }
    
}

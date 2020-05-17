#include "MotionEstimation.hpp"
#include "../../../Tooling/Validate.hpp"
#include "../../../ThirdParty/DX12/d3dx12.h"
#include "../RayTracing/ShaderBindingTable.hpp"
#include "../Swapchain.hpp"

namespace DXR
{
    
    MotionEstimator::MotionEstimator(GraphicsDevice& device)
    {
        this->video_device = device.GetVideoDevice();
        this->CreateVideoEncodingCommandQueue(device);
        this->CreateVideoEncodingCommandList(device);
        this->QueryMotionEstimationSupport();
        this->CreateMotionEstimator();
        this->CreateMotionVectorHeap();
        this->CreateResolvedMotionVectorTexture();
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
    
    //TODO(Tiago): Adapt this to use the swapchian size
    void MotionEstimator::CreateMotionVectorHeap()
    {
        D3D12_VIDEO_MOTION_VECTOR_HEAP_DESC MotionVectorHeapDesc = {
            0, // NodeIndex
            DXGI_FORMAT_NV12,
            D3D12_VIDEO_MOTION_ESTIMATOR_SEARCH_BLOCK_SIZE_16X16,
            D3D12_VIDEO_MOTION_ESTIMATOR_VECTOR_PRECISION_QUARTER_PEL,
            {1920, 1080, 640, 480} // D3D12_VIDEO_SIZE_RANGE
        };
        DXCall(video_device->CreateVideoMotionVectorHeap(
                                                         &MotionVectorHeapDesc,
                                                         nullptr,
                                                         IID_PPV_ARGS(&vector_heap)));
    }
    
    //TODO(Tiago): Adapt this to use the swapchian size
    void MotionEstimator::CreateResolvedMotionVectorTexture()
    {
        CD3DX12_RESOURCE_DESC resolvedMotionVectorDesc =
            CD3DX12_RESOURCE_DESC::Tex2D(
                                         DXGI_FORMAT_R16G16_SINT,
                                         Align(1920, 16) / 16, // This example uses a 16x16 block size. Pixel width and height
                                         Align(1080, 16) / 16, // are adjusted to store the vectors for those blocks.
                                         1, // ArraySize
                                         1  // MipLevels
                                         );
        
		D3D12_HEAP_PROPERTIES heap_properties = {};
		heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_properties.CreationNodeMask = 1;
		heap_properties.VisibleNodeMask = 1;
        
        DXCall((*GraphicsDevice::Device)->CreateCommittedResource(
                                                                  &heap_properties,
                                                                  D3D12_HEAP_FLAG_NONE,
                                                                  &resolvedMotionVectorDesc,
                                                                  D3D12_RESOURCE_STATE_COMMON,
                                                                  nullptr,
                                                                  IID_PPV_ARGS(&resolved_motion_vectors)));
        resolved_motion_vectors->SetName(L"Motion Vector Texture");
    }
    
    void MotionEstimator::CreateVideoEncodingCommandList(GraphicsDevice& device)
    {
        command_list = VideoEncodeCommandList(device);
    }
    
    void MotionEstimator::CreateVideoEncodingCommandQueue(GraphicsDevice& device)
    {
        command_queue = new VideoEncodeCommandQueue(device);
    }
    
    
    //TODO(Tiago): Backbuffer needs to be converted to NV12 before its estimation can happen
    void MotionEstimator::EstimateMotion(GraphicsDevice& device, Swapchain& swapchain)
    {
        const D3D12_VIDEO_MOTION_ESTIMATOR_OUTPUT output = {vector_heap.Get()};
        D3D12_VIDEO_MOTION_ESTIMATOR_INPUT input = {};
        input.InputSubresourceIndex = 0;
        input.pHintMotionVectorHeap = nullptr;
        input.pInputTexture2D = swapchain.GetCurrentBackBuffer().GetResource();
        input.pReferenceTexture2D = swapchain.GetNonCurrentBackbufferResource();
        input.ReferenceSubresourceIndex = 0;
        command_list->EstimateMotion(motion_estimator.Get(),&output,&input);
        const D3D12_RESOLVE_VIDEO_MOTION_VECTOR_HEAP_OUTPUT outputArgs = {
            resolved_motion_vectors.Get(),
            {}};
        const D3D12_RESOLVE_VIDEO_MOTION_VECTOR_HEAP_INPUT inputArgs = {
            vector_heap.Get(),
            swapchain.GetBackbufferResolution().Width,
            swapchain.GetBackbufferResolution().Height
        };
        command_list->ResolveMotionVectorHeap(&outputArgs,&inputArgs);
        DXCall(command_list->Close());
        command_queue->ExecuteCommandList(command_list);
    }
    
}

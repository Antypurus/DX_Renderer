#pragma once

#include <directxmath.h>

namespace DXR
{
    
    using namespace DirectX;
    
    struct Camera
    {
        XMFLOAT3 position = {0,0,0};
        XMFLOAT3 view_direction = {0,0,1};
        XMFLOAT3 up_direction = {0,1,0};
        
        Camera(const XMFLOAT3& position = {0,0,1}, const XMFLOAT3& view_direction = {0,0,-1});
        XMMATRIX ViewMatrix() const;
    };
    
}
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
        
        XMFLOAT3 right_direction = {0,0,0};
        
        mutable bool has_changed = true;
        mutable XMMATRIX view_matrix;
        
        float pitch = 0; //NOTE(Tiago): x-axis rotation
        float yaw = 0;   //NOTE(Tiago): y-axis rotation
        
        Camera(const XMFLOAT3& position = {0,0,1}, const XMFLOAT3& view_direction = {0,0,-1});
        XMMATRIX ViewMatrix() const;
        void Rotate();
        void Rotate(float pitch, float yaw);
        void DeltaRotate(float pitch_delta = 0, float yaw_delta = 0);
        void Move(float x_delta = 0, float y_delta =0, float z_delta = 0);
        void SetPosition(float x, float y, float z);
        void Forward(float intensity = 1.0f);
        void Backward(float intensity = 1.0f);
        void Left(float intensity = 1.0f);
        void Right(float intensity = 1.0f);
        
        private:
        void UpdateRightDirection();
    };
    
}

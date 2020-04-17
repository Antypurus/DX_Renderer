#define _USE_MATH_DEFINES

#include "Camera.hpp"
#include <cmath>

namespace DXR
{
    
#define ToRadian(degree) ((degree) * (M_PI / 180.0f))
    
    Camera::Camera(const XMFLOAT3& position, const XMFLOAT3& view_direction)
    {
        this->position = position;
        this->view_direction = view_direction;
    }
    
    XMMATRIX Camera::ViewMatrix() const
    {
        if(has_changed)
        {
            XMVECTOR focus_position = {
                position.x + view_direction.x,
                position.y + view_direction.y,
                position.z + view_direction.z,1.0f};
            view_matrix = XMMatrixLookAtLH({position.x,position.y,position.z,1.0f},
                                           focus_position,
                                           {up_direction.x,up_direction.y,up_direction.z,1.0f});
            has_changed = false;
        }
        return view_matrix;
    }
    
    void Camera::Rotate(float pitch_delta, float yaw_delta)
    {
        has_changed = true;
        
        pitch += pitch_delta;
        if(pitch > 90.0f)
        {
            pitch_delta = 90.0f - pitch;
            pitch = 90.0f;
        }else if(pitch < -90.0f)
        {
            pitch_delta = -90.0 - pitch;
            pitch = -90.0f;
        }
        
        yaw += yaw_delta;
        
        XMVECTOR rotation_quat = XMQuaternionRotationRollPitchYaw(ToRadian(pitch_delta),ToRadian(yaw_delta),0);
        XMVECTOR view_direction_vector = {
            view_direction.x,
            view_direction.y,
            view_direction.z,
            1.0f
        };
        view_direction_vector = XMVector3Rotate(view_direction_vector,rotation_quat);
        XMStoreFloat3(&view_direction,view_direction_vector); //NOTE(Tiago): Converts the vector to a float3
    }
    
    void Camera::Move(float x_delta, float y_delta, float z_delta)
    {
        has_changed = true;
        position = {position.x + x_delta, position.y + y_delta, position.z + z_delta};
    }
    
}
#define _USE_MATH_DEFINES

#include "Camera.hpp"
#include "../Core/Windows Abstractions/Window.hpp"

#include <cmath>

namespace DXR
{
    
#define ToRadian(degree) ((degree) * (M_PI / 180.0f))
    
    Camera::Camera(const XMFLOAT3& position, const XMFLOAT3& view_direction)
    {
        this->position = position;
        this->view_direction = view_direction;
        UpdateRightDirection();
        HookControlls();
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
    
    void Camera::Rotate()
    {
        has_changed = true;
        view_direction = {0,0,1};//NOTE(Tiago): Might need to store the initial value for the view direction such that i can reset it without issues
        XMVECTOR rotation_quat = XMQuaternionRotationRollPitchYaw(ToRadian(pitch),ToRadian(yaw),0);
        XMVECTOR view_direction_vector = {
            view_direction.x,
            view_direction.y,
            view_direction.z,
            1.0f
        };
        view_direction_vector = XMVector3Rotate(view_direction_vector,rotation_quat);
        XMStoreFloat3(&view_direction,view_direction_vector); //NOTE(Tiago): Converts the vector to a float3
        UpdateRightDirection();
    }
    
    void Camera::Rotate(float pitch, float yaw)
    {
        has_changed = true;
        if(pitch > 90)
        {
            pitch = 90;
        }else if(pitch < -90)
        {
            pitch = -90;
        }
        float pitch_delta = pitch - this->pitch;
        float yaw_delta = yaw = this->yaw;
        this->pitch = pitch;
        this->yaw = yaw;
        XMVECTOR rotation_quat = XMQuaternionRotationRollPitchYaw(ToRadian(pitch_delta),ToRadian(yaw_delta),0);
        XMVECTOR view_direction_vector = {
            view_direction.x,
            view_direction.y,
            view_direction.z,
            1.0f
        };
        view_direction_vector = XMVector3Rotate(view_direction_vector,rotation_quat);
        XMStoreFloat3(&view_direction,view_direction_vector); //NOTE(Tiago): Converts the vector to a float3
        UpdateRightDirection();
    }
    
    void Camera::DeltaRotate(float pitch_delta, float yaw_delta)
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
        UpdateRightDirection();
    }
    
    void Camera::Move(float x_delta, float y_delta, float z_delta)
    {
        has_changed = true;
        position = {position.x + x_delta, position.y + y_delta, position.z + z_delta};
    }
    
    void Camera::SetPosition(float x, float y, float z)
    {
        has_changed = true;
        position = {x,y,z};
    }
    
    void Camera::UpdateRightDirection()
    {
        XMVECTOR right_direction_vector;
        XMVECTOR view_direction_vector = {
            view_direction.x,
            view_direction.y,
            view_direction.z
        };
        XMVECTOR up_direction_vector = {
            up_direction.x,
            up_direction.y,
            up_direction.z
        };
        right_direction_vector = XMVector3Cross(view_direction_vector,up_direction_vector);
        XMStoreFloat3(&right_direction, right_direction_vector);
    }
    
    void Camera::Forward(float intensity)
    {
        has_changed = true;
        position = {
            position.x + view_direction.x * intensity,
            position.y + view_direction.y * intensity,
            position.z + view_direction.z * intensity
        };
    }
    
    void Camera::Backward(float intensity)
    {
        has_changed = true;
        position = {
            position.x - view_direction.x * intensity,
            position.y - view_direction.y * intensity,
            position.z - view_direction.z * intensity
        };
    }
    
    void Camera::Left(float intensity)
    {
        has_changed = true;
        position = {
            position.x - right_direction.x * intensity,
            position.y - right_direction.y * intensity,
            position.z - right_direction.z * intensity
        };
    }
    
    void Camera::Right(float intensity)
    {
        has_changed = true;
        position = {
            position.x + right_direction.x * intensity,
            position.y + right_direction.y * intensity,
            position.z + right_direction.z * intensity
        };
    }
    
    void Camera::HookControlls()
    {
        this->HookMouseControlls();
        this->HookKeyboardControlls();
    }
    
    void Camera::HookMouseControlls()
    {
		Window* window = Window::GetCurrentWindowHandle();
		WindowEventMessageCallback callback;
		callback.message = WM_KEYDOWN;
		callback.module = "Camera";
		callback.callback = [this](HWND window_instance, UINT message, WPARAM wParam, LPARAM lParam)
		{
		};
		window->RegisterWindowEventCallback(WM_KEYDOWN, callback);
    }
    
    void Camera::HookKeyboardControlls()
    {
        Window* window = Window::GetCurrentWindowHandle();
        WindowEventMessageCallback callback;
        callback.message = WM_KEYDOWN;
        callback.module = "Camera";
        callback.callback = [this](HWND window_instance, UINT message, WPARAM key, LPARAM lParam)
        {
            switch(key)
            {
                case(W_KEY):this->Forward(this->keyboard_intensity);break;
                case(S_KEY):this->Backward(this->keyboard_intensity);break;
                case(A_KEY):this->Left(this->keyboard_intensity);break;
                case(D_KEY):this->Right(this->keyboard_intensity);break;
                default:break;
            }
        };
        window->RegisterWindowEventCallback(WM_KEYDOWN,callback);
    }
}

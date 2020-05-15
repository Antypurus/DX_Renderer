#define _USE_MATH_DEFINES

#include "Camera.hpp"
#include "../Core/Windows Abstractions/Window.hpp"
#include "../Tooling/Log.hpp"

#include <cmath>
#include <Windowsx.h>
#include <thread>

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
		if (has_changed)
		{
			XMVECTOR focus_position = {
				view_direction.x,
				view_direction.y,
				view_direction.z,1.0f };
			view_matrix = XMMatrixLookToLH({ position.x,position.y,position.z,1.0f },
                                           focus_position,
                                           { up_direction.x,up_direction.y,up_direction.z,1.0f });
			has_changed = false;
		}
		return view_matrix;
	}
    
	void Camera::Rotate()
	{
		has_changed = true;
		view_direction = { 0,0,1 };//NOTE(Tiago): Might need to store the initial value for the view direction such that i can reset it without issues
		XMVECTOR rotation_quat = XMQuaternionRotationRollPitchYaw(ToRadian(pitch), ToRadian(yaw), 0);
		XMVECTOR view_direction_vector = {
			view_direction.x,
			view_direction.y,
			view_direction.z,
			1.0f
		};
		view_direction_vector = XMVector3Rotate(view_direction_vector, rotation_quat);
		XMStoreFloat3(&view_direction, view_direction_vector); //NOTE(Tiago): Converts the vector to a float3
		UpdateRightDirection();
	}
    
	void Camera::Rotate(float pitch, float yaw)
	{
		/*
		has_changed = true;
		if (pitch > CAM_MAX_PITCH)
		{
			pitch = CAM_MAX_PITCH;
		}
		else if (pitch < -CAM_MAX_PITCH)
		{
			pitch = -CAM_MAX_PITCH;
		}
		*/
		float pitch_delta = pitch - this->pitch;
		float yaw_delta = yaw = this->yaw;
		this->pitch = pitch;
		this->yaw = yaw;
		XMVECTOR rotation_quat = XMQuaternionRotationRollPitchYaw(ToRadian(pitch_delta), ToRadian(yaw_delta), 0);
		XMVECTOR view_direction_vector = {
			view_direction.x,
			view_direction.y,
			view_direction.z,
			1.0f
		};
		view_direction_vector = XMVector3Rotate(view_direction_vector, rotation_quat);
		XMStoreFloat3(&view_direction, view_direction_vector); //NOTE(Tiago): Converts the vector to a float3
		UpdateRightDirection();
	}
    
	void Camera::DeltaRotate(float pitch_delta, float yaw_delta)
	{
		has_changed = true;
		yaw += yaw_delta;
		pitch += pitch_delta;
		if (pitch > CAM_MAX_PITCH)
		{
            pitch = CAM_MAX_PITCH;
		}
		else if (pitch < -CAM_MAX_PITCH)
		{
			pitch = -CAM_MAX_PITCH;
		}
        
		XMVECTOR rotation_quat = XMQuaternionRotationRollPitchYaw(ToRadian(pitch), ToRadian(yaw), 0);
		XMVECTOR view_direction_vector = {0,0,1,0};
		view_direction_vector = XMVector3Rotate(view_direction_vector, rotation_quat);
		XMStoreFloat3(&view_direction, view_direction_vector); //NOTE(Tiago): Converts the vector to a float3

		UpdateRightDirection();
	}
    
	void Camera::Move(float x_delta, float y_delta, float z_delta)
	{
		has_changed = true;
		position = { position.x + x_delta, position.y + y_delta, position.z + z_delta };
	}
    
	void Camera::SetPosition(float x, float y, float z)
	{
		has_changed = true;
		position = { x,y,z };
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
		right_direction_vector = XMVector3Cross(view_direction_vector, up_direction_vector);
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
			position.x + right_direction.x * intensity,
			position.y + right_direction.y * intensity,
			position.z + right_direction.z * intensity
		};
	}
    
	void Camera::Right(float intensity)
	{
		has_changed = true;
		position = {
			position.x - right_direction.x * intensity,
			position.y - right_direction.y * intensity,
			position.z - right_direction.z * intensity
		};
	}
    
	void Camera::HookControlls()
	{
		this->HookMouseControlls();
		this->HookKeyboardControlls();
	}
    
	void Camera::HookMouseControlls()
	{
		//TODO(Tiago): What is the point of having the callback receive the message type? It is just creating confusion! refactor this!
		//NOTE(Tiago): Register callback for when right mouse button is pressed
		Window* window = Window::GetCurrentWindowHandle();
		WindowEventMessageCallback mouse_button_down_callback;
		//mouse_button_down_callback.message = WM_LBUTTONDOWN;
		mouse_button_down_callback.module = "Camera";
		mouse_button_down_callback.callback = [this](HWND window_instance, UINT message, WPARAM wParam, LPARAM lParam)
		{
			this->is_mouse_button_pressed = true;
            int x_mouse_pos = GET_X_LPARAM(lParam);
			int y_mouse_pos = GET_Y_LPARAM(lParam);
            this->previous_mouse_x = x_mouse_pos;
            this->previous_mouse_y = y_mouse_pos;
		};
		window->RegisterWindowEventCallback(WM_RBUTTONDOWN, mouse_button_down_callback);
		//NOTE(Tiago): Register callback for when right mouse button is lifted
		WindowEventMessageCallback mouse_button_up_callback;
		//mouse_button_up_callback.message = WM_LBUTTONUP;
		mouse_button_up_callback.module = "Camera";
		mouse_button_up_callback.callback = [this](HWND window_instance, UINT message, WPARAM wParam, LPARAM lParam)
		{
			this->is_mouse_button_pressed = false;
		};
		window->RegisterWindowEventCallback(WM_RBUTTONUP, mouse_button_up_callback);
		//NOTE(Tiago): Register Callback for mouse movement
		WindowEventMessageCallback callback;
		//callback.message = WM_MOUSEMOVE;
		callback.module = "Camera";
		callback.callback = [this](HWND window_instance, UINT message, WPARAM wParam, LPARAM lParam)
		{
			int x_mouse_pos = GET_X_LPARAM(lParam);
			int y_mouse_pos = GET_Y_LPARAM(lParam);
			int x_delta = x_mouse_pos - this->previous_mouse_x;
			int y_delta = y_mouse_pos - this->previous_mouse_y;
			this->previous_mouse_x = x_mouse_pos;
			this->previous_mouse_y = y_mouse_pos;
			if (this->is_first_capture)
			{
				this->is_first_capture = false;
				return;
			}
			if (this->is_mouse_button_pressed)
			{
				//TODO(Tiago): This way the ammount moved will depend on screen size i think, which is overall not good, so i might need to fix that
				this->DeltaRotate(-y_delta * this->mouse_intensity, x_delta * this->mouse_intensity);
			}
		};
		window->RegisterWindowEventCallback(WM_MOUSEMOVE, callback);
	}
    
    //TODO(Tiago): The way this is done when a key is release the original key is not detected and as such it doesnt keep moving, seems the only way to make this way perfectly is to move key processing to a thread and have this part just handlw setting the keys to be up or down
	void Camera::HookKeyboardControlls()
	{
		Window* window = Window::GetCurrentWindowHandle();
        {
            WindowEventMessageCallback callback;
            callback.message = WM_KEYDOWN;
            callback.module = "Camera";
            callback.callback = [this](HWND window_instance, UINT message, WPARAM key, LPARAM lParam)
            {
                switch (key)
                {
                    case(W_KEY): {this->w_key_down = true; break; }
                    case(S_KEY): {this->s_key_down = true; break; }
                    case(A_KEY): {this->a_key_down = true; break; }
                    case(D_KEY): {this->d_key_down = true; break; }
                    default:break;
                }
            };
            window->RegisterWindowEventCallback(WM_KEYDOWN, callback);
		}
        {
            WindowEventMessageCallback callback;
            callback.message = WM_KEYUP;
            callback.module = "Camera";
            callback.callback = [this](HWND window_instance, UINT message, WPARAM key, LPARAM lParam)
            {
                switch (key)
                {
                    case(W_KEY): {this->w_key_down = false; break; }
                    case(S_KEY): {this->s_key_down = false; break; }
                    case(A_KEY): {this->a_key_down = false; break; }
                    case(D_KEY): {this->d_key_down = false; break; }
                    default:break;
                }
            };
            window->RegisterWindowEventCallback(WM_KEYUP, callback);
		}
        //TODO(Tiago): Not sure i like this way of handling this, need to find a better approach. In practice it works pretty damn smothly, but i dont like the code itself, its feels very unmodular, might need to write an input module once im done with the thesis
        std::thread camera_command_thread([this]()
                                          {
                                              while(Window::GetCurrentWindowHandle()->ShouldContinue)
                                              {
                                                  if(this->w_key_down)
                                                  {
                                                      this->Forward(this->keyboard_intensity);
                                                  }
                                                  if(this->s_key_down)
                                                  {
                                                      this->Backward(this->keyboard_intensity);
                                                  }
                                                  if(this->a_key_down)
                                                  {
                                                      this->Left(this->keyboard_intensity);
                                                  }
                                                  if(this->d_key_down)
                                                  {
                                                      this->Right(this->keyboard_intensity);
                                                  }
                                                  //TODO(Tiago): there should be a better way to handle this, than just put this thread to sleep as that essentially ties the keyboard sensitivity to the sleep ammount
												  Sleep(1);
                                              }
                                          });
        camera_command_thread.detach();
	}
}

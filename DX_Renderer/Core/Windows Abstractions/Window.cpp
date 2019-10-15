#include "Window.hpp"

namespace DXR
{
	LRESULT WindowMessageCallbackProcedure(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
	{
		switch (Message)
		{
		case WM_CLOSE:
			DestroyWindow(WindowHandle);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(WindowHandle, Message, WParam, LParam);
		}
		return 0;
	}

	Window* GetCurrentWindowHanlde()
	{
		return current_window;
	}

	Window* Window::GetCurrentWindowHandle()
	{
		return current_window;
	}

	HWND Window::GetWindowHandle()
	{
		return this->m_window_handle;
	}

	HINSTANCE Window::GetInstance()
	{
		return this->m_instance;
	}

	const std::string& Window::GetWindowTittle()
	{
		return this->m_window_tittle;
	}

	void Window::UpdateWindow()
	{
		MSG message;
		const BOOL updateStatus = GetMessage(&message, this->m_window_handle, 0, 0);
		TranslateMessage(&message);
		DispatchMessage(&message);
		if(updateStatus == 0 || updateStatus == -1)
		{
			this->ShouldContinue = false;
		}
	}

	Window::Window(HINSTANCE Instance, int CmdShow, Resolution Resolution, const std::string& WindowTittle)
		:m_instance(Instance), m_cmd_show(CmdShow), m_window_tittle(WindowTittle), m_window_resolution(Resolution)
	{
		// create the window class
		WNDCLASSEX windowClass;
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = 0;
		windowClass.lpfnWndProc = WindowMessageCallbackProcedure;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = this->m_instance;
		windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		windowClass.lpszMenuName = nullptr;
		windowClass.lpszClassName = this->m_window_tittle.c_str();
		windowClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

		// register the window class
		if(!RegisterClassEx(&windowClass))
		{
			this->ShouldContinue = false;
			MessageBox(nullptr, "Window Class Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
			throw std::exception("Failed to create window class");
		}

		// create the window
		this->m_window_handle = CreateWindowEx(
			WS_EX_CLIENTEDGE, 
			this->m_window_tittle.c_str(),
			this->m_window_tittle.c_str(), 
			WS_OVERLAPPEDWINDOW, 
			CW_USEDEFAULT, CW_USEDEFAULT,
			this->m_window_resolution.Width, this->m_window_resolution.Height, 
			nullptr, 
			nullptr,
			this->m_instance, 
			nullptr);
		
		//validate window creation
		if(this->m_window_handle == nullptr)
		{
			this->ShouldContinue = false;
			MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
			throw std::exception("Failed to Create Window!");
		}

		ShowWindow(this->m_window_handle, this->m_cmd_show);
		::UpdateWindow(this->m_window_handle);

		current_window = this;
	}
}

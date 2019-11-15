#include "Window.hpp"
#include "../../Tooling/Log.hpp"

namespace DXR
{
	LRESULT WindowMessageCallbackProcedure(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
	{
		switch(Message)
		{
			case WM_CLOSE:
				INFO_LOG(L"Receibed WM_CLOSE Window Message\n");
				current_window->HandleMessage(Message, WindowHandle, WParam, LParam);
				DestroyWindow(WindowHandle);
				current_window->ShouldContinue = false;
				break;
			case WM_DESTROY:
				INFO_LOG(L"Receibed WM_DESTROY Window Message\n");
				current_window->HandleMessage(Message, WindowHandle, WParam, LParam);
				PostQuitMessage(0);
				break;
			default:
				if(current_window)
					current_window->HandleMessage(Message, WindowHandle, WParam, LParam);
				return DefWindowProc(WindowHandle, Message, WParam, LParam);
		}
		return 0;
	}

	Window* GetCurrentWindowHandle()
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
		const BOOL updateStatus = PeekMessage(&message, this->m_window_handle, 0, 0, PM_REMOVE);
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	void Window::RegisterWindowEventCallback(UINT message, const WindowEventMessageCallback& callback)
	{
		INFO_LOG(L"Registering Window Message Event Callback Function\n");
		this->m_registered_window_event_callbacks.emplace(message, callback);
	}

	void Window::HandleMessage(UINT Message, HWND windowHandle, WPARAM WParam, LPARAM LParam)
	{
		auto [currentCallback, rangeEnd] = this->m_registered_window_event_callbacks.equal_range(Message);
		while(currentCallback != rangeEnd)
		{
			currentCallback->second.callback(windowHandle, Message, WParam, LParam);
			++currentCallback;
		}
	}

	Resolution Window::GetResolution() const
	{
		return this->m_window_resolution;
	}

	Window::Window(HINSTANCE Instance, int CmdShow, Resolution Resolution, const std::string& WindowTittle)
		:m_instance(Instance), m_cmd_show(CmdShow), m_window_tittle(WindowTittle), m_window_resolution(Resolution)
	{
		INFO_LOG(L"Starting Windows Creation Procces\n");
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

		INFO_LOG(L"Registering Window Class\n");
		// register the window class
		if(!RegisterClassEx(&windowClass))
		{
			ERROR_LOG(L"Failed To Register Window Class\n");
			this->ShouldContinue = false;
			MessageBox(nullptr, "Window Class Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
			throw std::exception("Failed to create window class");
		}
		SUCCESS_LOG(L"Window Class Registered\n");

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

		INFO_LOG(L"Creating Win32 Window\n");
		//validate window creation
		if(this->m_window_handle == nullptr)
		{
			ERROR_LOG(L"Failed To Create Win32 Window\n");
			this->ShouldContinue = false;
			MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
			throw std::exception("Failed to Create Window!");
		}
		SUCCESS_LOG(L"Win32 Windows Created\n");

		ShowWindow(this->m_window_handle, this->m_cmd_show);
		::UpdateWindow(this->m_window_handle);

		current_window = this;
		SUCCESS_LOG(L"Window Creation Completed\n");
	}

}

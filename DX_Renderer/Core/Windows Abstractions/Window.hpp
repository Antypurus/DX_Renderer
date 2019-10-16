#pragma once

#include <string>
#include <functional>
#include <Windows.h>
#include <unordered_map>

/**
 * Defines a variable that should not be available outside of the translation module it was define in
 */
#define internal static

namespace DXR
{
	internal LRESULT CALLBACK WindowMessageCallbackProcedure(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);
	
	struct Resolution
	{
		UINT16 Width;
		UINT16 Height;
	};

	struct WindowEventMessageCallback
	{
		std::function<void(HWND, UINT, WPARAM, LPARAM)> callback;
		UINT message;
		std::string module;
	};
	
	struct Window
	{
		// public and private data fields
	public:
		bool ShouldContinue = true;
	private:
		HWND m_window_handle = nullptr;
		HINSTANCE m_instance = nullptr;
		int m_cmd_show = 0;

		std::string m_window_tittle;
		Resolution m_window_resolution = {};

		std::unordered_multimap<UINT, WindowEventMessageCallback> m_registered_window_event_callbacks;
		
		// public and private methods
	public:
		static Window* GetCurrentWindowHandle();
		
		Window(HINSTANCE Instance,int CmdShow,Resolution Resolution,const std::string& WindowTittle);
		HWND GetWindowHandle();
		HINSTANCE GetInstance();
		const std::string& GetWindowTittle();
		void UpdateWindow();
		void RegisterWindowEventCallback(UINT message,const WindowEventMessageCallback& callback);
		void HandleMessage(UINT Message, HWND windowHandle, WPARAM WParam, LPARAM LParam);
	};

	internal Window* current_window = nullptr;

	Window* GetCurrentWindowHandle();
}


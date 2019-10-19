#include <Windows.h>
#include "Core/Windows Abstractions/Window.hpp"
#include "Core/Components/GraphicsDevice.hpp"

int WINAPI CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
	// debug console and shit
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	DXR::GraphicsDevice device;
	auto devices = device.GetGraphicsAdapterList();
	for (auto& adapter : devices)
	{
		DXGI_ADAPTER_DESC description;
		adapter->GetDesc(&description);
		wprintf(L"%s\n", description.Description);
	}
	
	DXR::Window window{ hInstance,nCmdShow,{1280,720},"DX Renderer" };
	while(window.ShouldContinue)
	{
		window.UpdateWindow();
	}
	return 0;
}
#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "../Core/Components/Resource/DescriptorHeap.hpp"
#include "../ThirdParty/imgui/imgui.h"

namespace DXR
{
	struct GraphicsCommandList;
	struct Swapchain;
	struct Window;
	struct GraphicsDevice;
	
	class GUI
	{
	public:
	private:
		DescriptorHeap m_cbv_heap;
	public:
		GUI(GraphicsDevice& Device, Window& Window, Swapchain& swapchain);
		~GUI();
		void StartFrame();
		void Render(GraphicsCommandList& CommandList) const;
	private:
		void InitImGUI(GraphicsDevice& Device, Window& Window, Swapchain& swapchain);
		void Bind(GraphicsDevice& Device, Window& Window, Swapchain& swapchain);
		void RegisterWindowHook(Window& window);
	};
}

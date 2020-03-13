#include "GUI.hpp"
#include "../ThirdParty/imgui/imgui_impl_win32.h"
#include "../ThirdParty/imgui/imgui_impl_dx12.h"
#include "../Core/Components/GraphicsDevice.hpp"
#include "../Core/Components/Swapchain.hpp"
#include "../Core/Windows Abstractions/Window.hpp"
#include "../Core/Components/Resource/DescriptorHeap.hpp"
#include "../Core/Components/Command List/GraphicsCommandList.hpp"

namespace DXR
{
	GUI::GUI(GraphicsDevice& Device, Window& Window, Swapchain& swapchain)
	{
		this->m_cbv_heap = Device.CreateConstantBufferDescriptorHeap(1);
		this->InitImGUI(Device, Window,swapchain);
	}

	GUI::~GUI()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void GUI::StartFrame()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void GUI::Render(GraphicsCommandList& CommandList) const
	{
		CommandList.BindDescriptorHeap(this->m_cbv_heap);
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), CommandList.GetRAWInterface());
	}

	void GUI::InitImGUI(GraphicsDevice& Device, Window& Window, Swapchain& swapchain)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		this->Bind(Device, Window, swapchain);
	}

	void GUI::Bind(GraphicsDevice& Device, Window& Window, Swapchain& swapchain)
	{
		ImGui_ImplWin32_Init(Window.GetWindowHandle());
		ImGui_ImplDX12_Init(Device.GetRawInterface(), 2,
			swapchain.m_backbuffer_format, this->m_cbv_heap.GetRAWInterface(),
			this->m_cbv_heap.GetCPUHeapHandle(),
			this->m_cbv_heap.GetGPUHeapHandle());
	}
}

#define NOMINMAX

#include <Windows.h>
#include <cmath>
#include "VXGI.hpp"
#include "../Core/Components/GraphicsDevice.hpp"
#include "../Core/Components/Command Queue/GraphicsCommandQueue.hpp"
#include "../Tooling/Log.hpp"

namespace DXR
{
	SceneVoxelizer::SceneVoxelizer(GraphicsDevice& Device)
	{
		this->CreateRendererInterface(Device);
	}

	void SceneVoxelizer::CreateRendererInterface(GraphicsDevice& Device)
	{
		this->renderer_interface  = new NVRHI::RendererInterfaceD3D12(&error_handler,Device.GetRawInterface(),Device.GetGraphicsCommandQueue().GetCommandQueueRawPtr());
	}

	void SceneVoxelizer::CreateVXGIMainObject(GraphicsDevice& device)
	{
		return;
	}

	void ErrorCallbackHandler::signalError(const char* file, int line, const char* errorDesc)
	{
		//TODO(Tiago): Make this your own, right now its just copied and shit

		char buffer[4096];
		int length = (int)strlen(errorDesc);
		length = std::min(length, 4000); // avoid a "buffer too small" exception for really long error messages
		sprintf_s(buffer, "%s:%i\n%.*s", file, line, length, errorDesc);

		OutputDebugStringA(buffer);
		OutputDebugStringA("\n");
		MessageBoxA(NULL, buffer, "ERROR", MB_ICONERROR | MB_OK);
	}

}
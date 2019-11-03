#include "Debug.hpp"
#include <d3d12sdklayers.h>
#include <stdio.h>

namespace DXR
{
	Debug Debug::DebugInterface = {};

	Debug::Debug()
	{
		this->CreateLogTerminal();
		this->EnableD3D12DebugLayer();
	}

	void Debug::EnableD3D12DebugLayer()
	{
#ifndef NDEBUG
		WRL::ComPtr<ID3D12Debug> debugInterface;
		D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
		debugInterface->EnableDebugLayer();
#endif
	}

	void Debug::CreateLogTerminal()
	{
#ifndef NDEBUG
		AllocConsole();
		FILE* input = freopen("CONIN$", "r", stdin);
		FILE* output = freopen("CONOUT$", "w", stdout);
		FILE* error = freopen("CONOUT$", "w", stderr);
#endif
	}
}
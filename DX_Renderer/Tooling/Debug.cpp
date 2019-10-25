#include "Debug.hpp"
#include <d3d12sdklayers.h>

namespace DXR
{
	Debug::Debug()
	{
		this->CreateLogTerminal();
		this->EnableD3D12DebugLayer();
	}

	void Debug::EnableD3D12DebugLayer()
	{
#ifdef DEBUG
		WRL::ComPtr<ID3D12Debug> debugInterface;
		D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
		debugInterface->EnableDebugLayer();
#endif
	}
	void Debug::CreateLogTerminal()
	{
#if DEBUG
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
#endif
	}
}
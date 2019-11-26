#include "Debug.hpp"
#include <d3d12sdklayers.h>
#include <stdio.h>

namespace DXR
{
	Debug Debug::DebugInterface = {};
	HANDLE Debug::ConsoleHandle = {};

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
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );//TODO: move this to its own function
#endif
	}

	void Debug::CreateLogTerminal()
	{
#ifndef NDEBUG
		AllocConsole();
		FILE* input = freopen("CONIN$", "r", stdin);
		FILE* output = freopen("CONOUT$", "w", stdout);
		FILE* error = freopen("CONOUT$", "w", stderr);
		Debug::ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	}
}
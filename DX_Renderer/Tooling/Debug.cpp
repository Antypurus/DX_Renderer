#include "Debug.hpp"
#include <d3d12sdklayers.h>
#include <stdio.h>

namespace DXR
{
	Debug Debug::DebugInterface = {};
	HANDLE Debug::ConsoleHandle = {};

	Debug::~Debug()
	{
		fclose(this->stdin_stream);
		fclose(this->stdout_stream);
		fclose(this->stderr_stream);
	}

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
		freopen_s(&this->stdin_stream,"CONIN$", "r", stdin);
		freopen_s(&this->stdout_stream,"CONOUT$", "w", stdout);
		freopen_s(&this->stderr_stream,"CONOUT$", "w", stderr);
		Debug::ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	}
}
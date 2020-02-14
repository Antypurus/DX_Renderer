#pragma once

#include <wrl.h>
#include <string>

namespace DXR
{

	using namespace Microsoft;

	struct Debug
	{
	public:
		~Debug();
		static Debug DebugInterface;
		static HANDLE ConsoleHandle;
	private:
		FILE* stdin_stream = nullptr;
		FILE* stdout_stream = nullptr;
		FILE* stderr_stream = nullptr;
	private:
		Debug();
		void EnableD3D12DebugLayer();
		void CreateLogTerminal();
	};
}

#pragma once

#include <wrl.h>
#include <string>

namespace DXR
{

	using namespace Microsoft;

	struct Debug
	{
	public:
		static Debug DebugInterface;
		void Log(std::string message);
	private:
		Debug();
		void EnableD3D12DebugLayer();
		void CreateLogTerminal();
	};
}

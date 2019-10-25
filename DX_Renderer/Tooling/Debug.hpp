#pragma once

#include <WRL.h>

namespace DXR
{

	using namespace Microsoft;

	struct Debug
	{
	public:
		static Debug DebugInterface;
	private:
		Debug();
		void EnableD3D12DebugLayer();
		void CreateLogTerminal();
	};
}

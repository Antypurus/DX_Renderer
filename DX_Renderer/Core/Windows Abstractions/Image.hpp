#pragma once
#include <wincodec.h>
#include <wrl.h>

namespace DXR
{

	using namespace Microsoft;
	
	struct Image
	{
	public:
	private:
		WRL::ComPtr<IWICImagingFactory2> m_imaging_factory;
	public:
		Image();
	private:
	};
}
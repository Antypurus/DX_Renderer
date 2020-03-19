#include "../../Tooling/Validate.hpp"
#include "Image.hpp"

namespace DXR
{
	Image::Image()
	{
		DXCall(CoInitialize(NULL));
		DXCall(CoCreateInstance(CLSID_WICImagingFactory2, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&this->m_imaging_factory)));
	}
}

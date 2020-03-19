#include "../../Tooling/Validate.hpp"
#include "TextureFS.hpp"

namespace DXR
{
	Image Image::m_instance = {};
	
	Image::~Image()
	{
		CoUninitialize();
	}

	Image& Image::GetInstance()
	{
		return Image::m_instance;
	}

	void Image::Decode(const std::wstring& Filepath)
	{
		Image instance = GetInstance();
		WRL::ComPtr<IWICBitmapDecoder> decoder;
		DXCall(instance.m_imaging_factory->CreateDecoderFromFilename(Filepath.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, decoder.GetAddressOf()));
	}

	Image::Image()
	{
		this->CreateImagingFactory();
	}

	void Image::CreateImagingFactory()
	{
		DXCall(CoInitializeEx(NULL, COINIT_MULTITHREADED));
		DXCall(CoCreateInstance(CLSID_WICImagingFactory2, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&this->m_imaging_factory)));
	}
}

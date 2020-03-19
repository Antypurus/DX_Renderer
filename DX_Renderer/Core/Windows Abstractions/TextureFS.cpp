#include "../../Tooling/Validate.hpp"
#include "TextureFS.hpp"

namespace DXR
{
	TextureFS TextureFS::m_instance = {};

	TextureData::TextureData(WRL::ComPtr<IWICBitmapFrameDecode>& TextureFrame)
	{
	}

	TextureFS::~TextureFS()
	{
		CoUninitialize();
	}

	TextureFS& TextureFS::GetInstance()
	{
		return TextureFS::m_instance;
	}

	TextureData TextureFS::LoadTextureData(const std::wstring& Filepath)
	{
		TextureFS instance = GetInstance();
		WRL::ComPtr<IWICBitmapDecoder> decoder;
		DXCall(instance.m_imaging_factory->CreateDecoderFromFilename(Filepath.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, decoder.GetAddressOf()));
		
		WRL::ComPtr<IWICBitmapFrameDecode> frame;
		decoder->GetFrame(0, frame.GetAddressOf());
		return { frame };
	}

	TextureFS::TextureFS()
	{
		this->CreateImagingFactory();
	}

	void TextureFS::CreateImagingFactory()
	{
		DXCall(CoInitializeEx(NULL, COINIT_MULTITHREADED));
		DXCall(CoCreateInstance(CLSID_WICImagingFactory2, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&this->m_imaging_factory)));
	}
}

#pragma once
#include <string>
#include <wincodec.h>
#include <wrl.h>

namespace DXR
{
	using namespace Microsoft;

	struct TextureData
	{
	private:
		WRL::ComPtr<IWICBitmapFrameDecode> m_frame;
	};
	
	struct TextureFS
	{
	public:
	private:
		static TextureFS m_instance;
		WRL::ComPtr<IWICImagingFactory2> m_imaging_factory;
	public:
		~TextureFS();
		static TextureFS& GetInstance();
		static void LoadTextureData(const std::wstring& Filepath);
	private:
		TextureFS();
		void CreateImagingFactory();
	};
	
}
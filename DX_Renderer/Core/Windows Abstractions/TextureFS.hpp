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
		BYTE* m_texture_data = nullptr;
	public:
		TextureData(WRL::ComPtr<IWICBitmapFrameDecode>& TextureFrame);
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
		static TextureData LoadTextureData(const std::wstring& Filepath);
	private:
		TextureFS();
		void CreateImagingFactory();
	};
	
}
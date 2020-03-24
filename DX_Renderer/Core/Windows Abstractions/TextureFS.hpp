#pragma once
#include <memory>
#include <string>
#include <wincodec.h>
#include <wrl.h>

namespace DXR
{
	using namespace Microsoft;
	
	struct TextureData
	{
	private:
		// actual texture
		std::unique_ptr<BYTE[]> m_texture_data = nullptr;
		// metadata
		UINT8 m_bit_per_pixel;
		UINT64 m_image_size;
		UINT64 m_alignment;
		UINT64 m_width;
		UINT64 m_height;
		UINT16 m_mip_levels;
		WICPixelFormatGUID m_pixel_format;
	public:
		TextureData(WRL::ComPtr<IWICBitmapFrameDecode>& TextureFrame);
		const BYTE* const GetTextureData() const;
		UINT64 GetTextureSize() const;
	private:
		WRL::ComPtr<IWICFormatConverter> ConvertToFormat(WRL::ComPtr<IWICBitmapFrameDecode>& TextureFrame, const WICPixelFormatGUID& PixelFormat);
		bool CheckIfFormatIsSupported() const;
		bool CheckIfFormatIsSupported(const WICPixelFormatGUID& PixelFormat) const;
		WICPixelFormatGUID DetermineCompatiblePixelFormat() const;
		WICPixelFormatGUID DetermineCompatiblePixelFormat(const WICPixelFormatGUID& PixelFormat) const;
		UINT8 ComputerPixelFormatBitsPerPixel() const;
		UINT8 ComputerPixelFormatBitsPerPixel(const WICPixelFormatGUID& PixelFormat) const;
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
		IWICImagingFactory2* operator->() const;
		static UINT ComputeTextureFormatBitsPerPixel(const WICPixelFormatGUID& PixelFormat);
	private:
		TextureFS();
		void CreateImagingFactory();
	};
	
}
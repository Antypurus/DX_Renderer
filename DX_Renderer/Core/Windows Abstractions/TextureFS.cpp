#include "../../Tooling/Validate.hpp"
#include "TextureFS.hpp"
#include <cmath>

namespace DXR
{
	TextureFS TextureFS::m_instance = {};

	void TextureData::operator=(const TextureData& texture)
	{
		this->m_bit_per_pixel = texture.m_bit_per_pixel;
		this->m_image_size = texture.m_image_size;
		this->m_alignment = texture.m_alignment;
		this->m_width = texture.m_width;
		this->m_height = texture.m_height;
		this->m_mip_levels = texture.m_mip_levels;
		this->m_pixel_format = texture.m_pixel_format;

		//copy texture data from original to the new object
		this->m_texture_data = std::make_unique<BYTE[]>(this->m_image_size);
		memmove(this->m_texture_data.get(),texture.m_texture_data.get(),this->m_image_size);
	}

	TextureData::TextureData(WRL::ComPtr<IWICBitmapFrameDecode>& TextureFrame)
	{
		DXCall(TextureFrame->GetPixelFormat(&this->m_pixel_format));
		UINT width, height;
		DXCall(TextureFrame->GetSize(&width, &height));
		this->m_height = height;
		this->m_width = width;
		this->m_alignment = 0;

		if (!CheckIfFormatIsSupported())
		{
			const auto compatible_format = DetermineCompatiblePixelFormat();
			if (!CheckIfFormatIsSupported(compatible_format))
			{
				//ERROR(Tiago): Unsupported Texture Format
				ERROR_LOG("Unsupported Texture Format");
				__debugbreak();
				exit(-1);
			}

			//conver to new texture format
			auto converter = this->ConvertToFormat(TextureFrame, compatible_format);

			// texture information
			this->m_pixel_format = compatible_format;
			this->m_bit_per_pixel = this->ComputerPixelFormatBitsPerPixel();
			this->m_pixel_format = compatible_format;
			this->m_image_size = (this->m_width * this->m_height * this->m_bit_per_pixel) / 8;

			//allocate texture buffer
			this->m_texture_data = std::make_unique<BYTE[]>(this->m_image_size);

			// texture data must be extracted from the converter in this case
			DXCall(converter->CopyPixels(nullptr, (this->m_width * this->m_bit_per_pixel) / 8, this->m_image_size, this->m_texture_data.get()));
		}
		else
		{
			// texture information
			this->m_bit_per_pixel = this->ComputerPixelFormatBitsPerPixel();
			this->m_image_size = (this->m_width * this->m_height * this->m_bit_per_pixel) / 8;

			// allocate texture buffer
			this->m_texture_data = std::make_unique<BYTE[]>(this->m_image_size);

			// extract texture data from the texture frame
			DXCall(TextureFrame->CopyPixels(nullptr, (this->m_width * this->m_bit_per_pixel) / 8, this->m_image_size, (BYTE*)this->m_texture_data.get()));
		}
	}

	const BYTE* const TextureData::GetTextureData() const
	{
		return this->m_texture_data.get();
	}

	UINT64 TextureData::GetTextureSize() const
	{
		return this->m_image_size;
	}

	WICPixelFormatGUID TextureData::GetTextureFormat() const
	{
		return this->m_pixel_format;
	}

	UINT64 TextureData::GetWidth() const
	{
		return this->m_width;
	}

	UINT64 TextureData::GetHeight() const
	{
		return this->m_height;
	}

	UINT64 TextureData::GetBytesPerPixel() const
	{
		return this->m_bit_per_pixel/8;
	}

	UINT64 TextureData::CalculateRowPitch() const
	{
		return this->m_width * this->GetBytesPerPixel();
	}


	UINT64 TextureData::CalculateAlignedRowPitch() const
	{
		return std::lround(std::ceil(this->m_width * this->GetBytesPerPixel() / (double)256)) * 256;
	}
	
	UINT16 TextureData::GetMipLevelCount() const
	{
		return this->m_mip_levels;
	}

	WRL::ComPtr<IWICFormatConverter> TextureData::ConvertToFormat(WRL::ComPtr<IWICBitmapFrameDecode>& TextureFrame, const WICPixelFormatGUID& PixelFormat)
	{
		WRL::ComPtr<IWICFormatConverter> converter;
		DXCall(TextureFS::GetInstance()->CreateFormatConverter(converter.GetAddressOf()));

		BOOL isConvertible = FALSE;
		DXCall(converter->CanConvert(this->m_pixel_format, PixelFormat, &isConvertible));
		if (!isConvertible)
		{
			//ERROR(Tiago): Unsupported Texture Format
			ERROR_LOG("Unsupported Texture Format");
			__debugbreak();
			exit(-1);
		}
		DXCall(converter->Initialize(TextureFrame.Get(), PixelFormat, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom));
		return converter;
	}

	bool TextureData::CheckIfFormatIsSupported() const
	{
		return this->CheckIfFormatIsSupported(this->m_pixel_format);
	}

	bool TextureData::CheckIfFormatIsSupported(const WICPixelFormatGUID& PixelFormat) const
	{
		//NOTE(Tiago): pixel format type is defined in a weird way that seems to not allows for switch case statements
		if (PixelFormat == GUID_WICPixelFormat128bppRGBAFloat) return true;
		else if (PixelFormat == GUID_WICPixelFormat64bppRGBAHalf) return true;
		else if (PixelFormat == GUID_WICPixelFormat64bppRGBA) return true;
		else if (PixelFormat == GUID_WICPixelFormat32bppRGBA) return true;
		else if (PixelFormat == GUID_WICPixelFormat32bppBGRA) return true;
		else if (PixelFormat == GUID_WICPixelFormat32bppBGR) return true;
		else if (PixelFormat == GUID_WICPixelFormat32bppRGBA1010102XR) return true;
		else if (PixelFormat == GUID_WICPixelFormat32bppRGBA1010102) return true;
		else if (PixelFormat == GUID_WICPixelFormat16bppBGRA5551) return true;
		else if (PixelFormat == GUID_WICPixelFormat16bppBGR565) return true;
		else if (PixelFormat == GUID_WICPixelFormat32bppGrayFloat) return true;
		else if (PixelFormat == GUID_WICPixelFormat16bppGrayHalf) return true;
		else if (PixelFormat == GUID_WICPixelFormat16bppGray) return true;
		else if (PixelFormat == GUID_WICPixelFormat8bppGray) return true;
		else if (PixelFormat == GUID_WICPixelFormat8bppAlpha) return true;

		return false;
	}

	WICPixelFormatGUID TextureData::DetermineCompatiblePixelFormat() const
	{
		return this->DetermineCompatiblePixelFormat(this->m_pixel_format);
	}

	WICPixelFormatGUID TextureData::DetermineCompatiblePixelFormat(const WICPixelFormatGUID& PixelFormat) const
	{
		if (PixelFormat == GUID_WICPixelFormatBlackWhite) return GUID_WICPixelFormat8bppGray;
		else if (PixelFormat == GUID_WICPixelFormat1bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat2bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat4bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat8bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat2bppGray) return GUID_WICPixelFormat8bppGray;
		else if (PixelFormat == GUID_WICPixelFormat4bppGray) return GUID_WICPixelFormat8bppGray;
		else if (PixelFormat == GUID_WICPixelFormat16bppGrayFixedPoint) return GUID_WICPixelFormat16bppGrayHalf;
		else if (PixelFormat == GUID_WICPixelFormat32bppGrayFixedPoint) return GUID_WICPixelFormat32bppGrayFloat;
		else if (PixelFormat == GUID_WICPixelFormat16bppBGR555) return GUID_WICPixelFormat16bppBGRA5551;
		else if (PixelFormat == GUID_WICPixelFormat32bppBGR101010) return GUID_WICPixelFormat32bppRGBA1010102;
		else if (PixelFormat == GUID_WICPixelFormat24bppBGR) return GUID_WICPixelFormat32bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat24bppRGB) return GUID_WICPixelFormat32bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat32bppPBGRA) return GUID_WICPixelFormat32bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat32bppPRGBA) return GUID_WICPixelFormat32bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat48bppRGB) return GUID_WICPixelFormat64bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat48bppBGR) return GUID_WICPixelFormat64bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat64bppBGRA) return GUID_WICPixelFormat64bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat64bppPRGBA) return GUID_WICPixelFormat64bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat64bppPBGRA) return GUID_WICPixelFormat64bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat48bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (PixelFormat == GUID_WICPixelFormat48bppBGRFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (PixelFormat == GUID_WICPixelFormat64bppRGBAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (PixelFormat == GUID_WICPixelFormat64bppBGRAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (PixelFormat == GUID_WICPixelFormat64bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (PixelFormat == GUID_WICPixelFormat64bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (PixelFormat == GUID_WICPixelFormat48bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (PixelFormat == GUID_WICPixelFormat128bppPRGBAFloat) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (PixelFormat == GUID_WICPixelFormat128bppRGBFloat) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (PixelFormat == GUID_WICPixelFormat128bppRGBAFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (PixelFormat == GUID_WICPixelFormat128bppRGBFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (PixelFormat == GUID_WICPixelFormat32bppRGBE) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (PixelFormat == GUID_WICPixelFormat32bppCMYK) return GUID_WICPixelFormat32bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat64bppCMYK) return GUID_WICPixelFormat64bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat40bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat80bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
		else if (PixelFormat == GUID_WICPixelFormat32bppRGB) return GUID_WICPixelFormat32bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat64bppRGB) return GUID_WICPixelFormat64bppRGBA;
		else if (PixelFormat == GUID_WICPixelFormat64bppPRGBAHalf) return GUID_WICPixelFormat64bppRGBAHalf;
#endif

		return GUID_WICPixelFormatDontCare;
	}

	UINT8 TextureData::ComputerPixelFormatBitsPerPixel() const
	{
		return this->ComputerPixelFormatBitsPerPixel(this->m_pixel_format);
	}

	UINT8 TextureData::ComputerPixelFormatBitsPerPixel(const WICPixelFormatGUID& PixelFormat) const
	{
		return TextureFS::ComputeTextureFormatBitsPerPixel(PixelFormat);
	}

	TextureFS::~TextureFS()
	{
		this->m_imaging_factory.Reset();
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

	IWICImagingFactory2* DXR::TextureFS::operator->() const
	{
		return this->m_imaging_factory.Get();
	}

	UINT TextureFS::ComputeTextureFormatBitsPerPixel(const WICPixelFormatGUID& PixelFormat)
	{
		WRL::ComPtr<IWICComponentInfo> info;
		TextureFS::GetInstance().m_imaging_factory->CreateComponentInfo(PixelFormat, info.GetAddressOf());
		IWICPixelFormatInfo* pixelinfo = nullptr;
		info->QueryInterface(IID_IWICPixelFormatInfo, (LPVOID*)&pixelinfo);
		UINT result = 0;
		DXCall(pixelinfo->GetBitsPerPixel(&result));
		pixelinfo->Release();
		return result;
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

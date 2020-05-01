#include "Texture.hpp"

#include <thread>

#include "../GPU Buffers/GPUDefaultBuffer.hpp"
#include "../../GraphicsDevice.hpp"
#include "../../Command List/GraphicsCommandList.hpp"
#include "../../Fence.hpp"

namespace DXR
{
	static void WaitForDataTransfer(GraphicsDevice& device, Fence fence, TextureUploadBuffer* buffer)
	{
		fence.WaitForFence();
		//buffer->Evict(device);//TODO(Tiago): Figure out how to safely wait for data transfer to finish
	}
    
	Texture::Texture(const std::wstring& filepath, GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{
		this->m_texture_data = TextureFS::LoadTextureData(filepath);
		this->m_texture_format = this->DetermineTextureDataFormat();
		this->m_resource_description = this->CreateResourceDescription();
        
		this->CreateTextureBuffers(Device, CommandList);
		this->QueueUploadBufferForEviction(Device);
        
		this->CreateDescriptorHeap(Device);
		this->CreateShaderResourceViewDescription(Device);
        
		this->m_sampler = Sampler(Device);
	}
    
    Texture::Texture(const Texture& other)
    {
        this->m_texture_data = other.m_texture_data;
		this->m_texture_format = other.m_texture_format;
        this->m_resource_description = other.m_resource_description;
        this->m_srv_desc = other.m_srv_desc;
        this->m_heap = other.m_heap;
        this->m_upload_buffer = other.m_upload_buffer;
        this->m_texture_buffer = other.m_texture_buffer;
    }
    
	DescriptorHeap* Texture::GetSRVHeap()
	{
		return &this->m_heap;
	}
    
	DescriptorHeap* Texture::GetSamplerHeap()
	{
		return this->m_sampler.GetDescriptorHeap();
	}
    
	void Texture::CreateShaderResourceViewDescription(GraphicsDevice& Device) const
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		srv_desc.Format = this->m_texture_format;
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D = {};
		srv_desc.Texture2D.MipLevels = this->m_texture_data.GetMipLevelCount();
		srv_desc.Texture2D.MostDetailedMip = 0;
		srv_desc.Texture2D.ResourceMinLODClamp = 0;
		srv_desc.Texture2D.PlaneSlice = 0;
        
		Device->CreateShaderResourceView(this->m_texture_buffer->GetResource(),&srv_desc,this->m_heap[this->m_heap_index]);
	}
    
	void Texture::CreateDescriptorHeap(GraphicsDevice& device)
	{
		this->m_heap = SRHeapManager::GetManager().descriptor_heap;
		this->m_descriptor_heap = &this->m_heap;
		this->m_heap_index = SRHeapManager::GetManager().Allocate();
	}
    
	void Texture::QueueUploadBufferForEviction(GraphicsDevice& Device) const
	{
		auto fence = Device.CreateFence(0);
		fence.Advance();
		fence.Signal(Device.GetGraphicsCommandQueue());
		std::thread thread(WaitForDataTransfer, Device, fence, m_upload_buffer.get());
		thread.detach();
	}
    
	void Texture::CreateTextureBuffers(GraphicsDevice& Device, GraphicsCommandList& CommandList)
	{
		this->m_upload_buffer = std::make_shared<TextureUploadBuffer>(Device, CalculateBufferSize(this->m_texture_data), this->m_texture_data);
		this->m_texture_buffer = std::make_shared<GPUDefaultBuffer>(Device, CommandList, 1,this->m_texture_data.GetTextureSize(), this->m_resource_description);
        
		this->m_upload_buffer->UploadTextureDataToDefaultBuffer(CommandList, *this->m_texture_buffer, this->m_resource_description, this->m_texture_data.CalculateAlignedRowPitch());
	}
    
	DXGI_FORMAT Texture::DetermineTextureDataFormat() const
	{
		if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
        
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
		else if (this->m_texture_data.GetTextureFormat() == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;
        
		return DXGI_FORMAT_UNKNOWN;
	}
    
	D3D12_HEAP_PROPERTIES Texture::CreateResourceHeapDescription()
	{
		return {};
	}
    
	D3D12_RESOURCE_DESC Texture::CreateResourceDescription(D3D12_RESOURCE_FLAGS ResourceFlags)
	{
		D3D12_RESOURCE_DESC resource = {};
		resource.Alignment = 0;
		resource.DepthOrArraySize = 1;//For now only 2D textures
		resource.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resource.Flags = D3D12_RESOURCE_FLAG_NONE;
		resource.Format = this->m_texture_format;
		resource.Height = this->m_texture_data.GetHeight();
		resource.Width = this->m_texture_data.GetWidth();
		resource.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resource.MipLevels = 1;
		// For now no texture multisampling is allowed
		resource.SampleDesc.Count = 1;
		resource.SampleDesc.Quality = 0;
        
		return resource;
	}
    
	D3D12_CLEAR_VALUE Texture::CreateOptimizedClearValue()
	{
		return {};
	}
    
	void Texture::UploadTextureData(GraphicsCommandList& CommandList)
	{
		this->m_upload_buffer->CopyDataToGPUBuffer(CommandList, *this->m_texture_buffer);
	}
    
	UINT64 Texture::CalculateBufferSize(TextureData& data)
	{
		int textureHeapSize = ((((data.GetWidth() * data.GetBytesPerPixel()) + 255) & ~255) * (data.GetHeight() - 1)) + (data.GetWidth() * data.GetBytesPerPixel());
		return textureHeapSize;
	}
}

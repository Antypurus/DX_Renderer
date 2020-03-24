#pragma once
#include "../../../Windows Abstractions/TextureFS.hpp"

namespace  DXR
{

	struct Texture
	{
	public:
	private:
		TextureData m_texture_data;
	public:
	private:
		DXGI_FORMAT DetermineTextureDataFormat();
	};

}

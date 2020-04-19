#pragma once
#include "Shader.hpp"

namespace DXR
{
	struct PixelShader: public Shader
	{
        public:
        std::wstring filepath;
        std::wstring entrypoint;
        private:
        public:
		static PixelShader CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint);
        private:
		PixelShader(const std::wstring& filepath, const std::wstring& entrypoint);
	};
}

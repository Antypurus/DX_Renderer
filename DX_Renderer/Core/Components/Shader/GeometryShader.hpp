#pragma once
#include "Shader.hpp"

namespace DXR
{
	struct GeometryShader: public Shader
	{
        public:
        private:
        public:
        GeometryShader();
		static GeometryShader CompileShaderFromFile(const std::wstring& filename, const std::wstring& entryPoint);
        private:
	};
}

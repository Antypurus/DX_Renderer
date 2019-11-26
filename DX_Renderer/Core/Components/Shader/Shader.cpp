#include "Shader.hpp"
#include <d3dcompiler.h>

namespace DXR
{
	Shader::Shader(ShaderType shaderType)
	{
		this->m_shader_type = shaderType;
	}
}

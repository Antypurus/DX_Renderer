#include "Shader.hpp"
#include <d3dcompiler.h>
#include "../../../Tooling/Log.hpp"
#include "../../../Tooling/Validate.hpp"

namespace DXR
{
	D3D12_SHADER_BYTECODE Shader::GetShaderBytecode()
	{
		D3D12_SHADER_BYTECODE shader_bytecode = {};
		shader_bytecode.pShaderBytecode = this->m_shader_code->GetBufferPointer();
		shader_bytecode.BytecodeLength = this->m_shader_code->GetBufferSize();

		return shader_bytecode;
	}
	D3D12_SHADER_BYTECODE Shader::NoShaderBytecode()
	{
		D3D12_SHADER_BYTECODE bytecode = {};
		bytecode.BytecodeLength = 0;
		bytecode.pShaderBytecode = nullptr;

		return bytecode;
	}
	void Shader::CompileFromFile(const std::wstring& filename, const std::string& entryPoint)
	{
		ID3DBlob* shader_code;
		ID3DBlob* error_msg;
		UINT compilation_flags = 0;

	#ifndef NDEBUG
		compilation_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif

		DXCall(D3DCompileFromFile(filename.c_str(), nullptr, nullptr, entryPoint.c_str(), this->m_shader_type_name.c_str(), compilation_flags, 0, &shader_code, &error_msg));

		// validate compilation errors
		if(error_msg != nullptr)
		{
			ERROR_LOG((char*)error_msg->GetBufferPointer());
			error_msg->Release();
			MessageBox(NULL, "Failed To Compile Shader", "Error", MB_ICONEXCLAMATION | MB_OK);
			throw std::exception("Failed To Compile Shader");
		}

		this->m_shader_code.Reset();
		this->m_shader_code = WRL::ComPtr<ID3DBlob>(shader_code);
	}

	void Shader::Compile(const std::string& shaderCode, const std::string& entryPoint)
	{
		ID3DBlob* shader_code;
		ID3DBlob* error_msg;
		UINT compilation_flags = 0;

	#ifndef NDEBUG
		compilation_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif

		DXCall(D3DCompile(shaderCode.c_str(), shaderCode.length() + 1, nullptr, nullptr, nullptr, entryPoint.c_str(), this->m_shader_type_name.c_str(), compilation_flags, 0, &shader_code, &error_msg));

		// validate compilation errors
		if(error_msg != nullptr)
		{
			ERROR_LOG((char*)error_msg->GetBufferPointer());
			error_msg->Release();
			MessageBox(NULL, "Failed To Compile Shader", "Error", MB_ICONEXCLAMATION | MB_OK);
			throw std::exception("Failed To Compile Shader");
		}

		this->m_shader_code.Reset();
		this->m_shader_code = WRL::ComPtr<ID3DBlob>(shader_code);
	}

	Shader::Shader(ShaderType shaderType)
	{
		this->m_shader_type = shaderType;
		switch(this->m_shader_type)
		{
			case ShaderType::VertexShader:
			{
				this->m_shader_type_name = VERTEX_SHADER;
				break;
			}
			case ShaderType::HullShader:
			{
				this->m_shader_type_name = HULL_SHADER;
				break;
			}
			case ShaderType::DomainShader:
			{
				this->m_shader_type_name = DOMAIN_SHADER;
				break;
			}
			case ShaderType::GeometryShader:
			{
				this->m_shader_type_name = GEOMETRY_SHADER;
				break;
			}
			case ShaderType::PixelShader:
			{
				this->m_shader_type_name = PIXEL_SHADER;
				break;
			}
			case ShaderType::ComputerShader:
			{
				this->m_shader_type_name = COMPUTE_SHADER;
				break;
			}
			default:
				break;
		}
	}
}

#include "Shader.hpp"
#include <d3dcompiler.h>
#include "../../../Tooling/Log.hpp"
#include "../../../Tooling/Validate.hpp"

namespace DXR
{
    
	using namespace WRL;
    
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
	void Shader::CompileFromFile(const std::wstring& filename, const std::wstring& entryPoint)
	{
		auto code = ShaderCompiler::CompileFromFile(filename,entryPoint,this->m_shader_type_name);
		this->m_shader_code = WRL::ComPtr<IDxcBlob>(code);
	}
    
	Shader::Shader(ShaderType shaderType)
	{
		auto inst = ShaderCompiler::GetInstance();
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
			case ShaderType::RayTracingShader:
			{
				this->m_shader_type_name = RAYTRACING_SHADER;
				break;
			}
			default:
            break;
		}
	}
	
	ShaderCompiler ShaderCompiler::m_instance = ShaderCompiler::ShaderCompiler();
    
	ShaderCompiler ShaderCompiler::GetInstance()
	{
		return ShaderCompiler::m_instance;
	}
    
	IDxcBlob* ShaderCompiler::CompileFromFile(const std::wstring& Filepath, const std::wstring& Entrypoint, const std::wstring ShaderType)
	{
		auto encoding = m_shader_encoding;
		auto instance = ShaderCompiler::GetInstance();
        
		ComPtr<IDxcBlobEncoding> shader_code_blob;
		DXCall(instance.m_library->CreateBlobFromFile(Filepath.c_str(),&encoding,&shader_code_blob));
        
		ComPtr<IDxcOperationResult> result;
#ifndef NDEBUG
		//Note(Tiago): Include Handler Is Not Used ATM but it might be needed in the future
		ComPtr<IDxcBlob> debug_blob;
		LPWSTR debug_file = L"ShaderDebug";
		HRESULT hr = (instance.m_compiler->CompileWithDebug(shader_code_blob.Get(), Filepath.c_str(), Entrypoint.c_str(), ShaderType.c_str(), NULL, 0, NULL, 0, instance.m_include_handler.Get(), &result, &debug_file, debug_blob.GetAddressOf()));
        
		ComPtr<IDxcBlobEncoding> pErrors = nullptr;
		result->GetErrorBuffer(&pErrors);
		if(pErrors != nullptr)
		{
			if(pErrors->GetBufferSize() != 0)
			{
				ERROR_LOG((char*)pErrors->GetBufferPointer());
				MessageBox(NULL, "Failed To Compile Shader", "Error", MB_ICONEXCLAMATION | MB_OK);
				throw std::exception("Failed To Compile Shader");
			}
		}
#else
		//Note(Tiago): Include Handler Is Not Used ATM but it might be needed in the future
		DXCall(instance.m_compiler->Compile(shader_code_blob.Get(), Filepath.c_str(), Entrypoint.c_str(), ShaderType.c_str(), NULL, 0, NULL, 0, instance.m_include_handler.Get(), &result));
#endif
		IDxcBlob* code;
		result->GetResult(&code);
		auto ptr  = code->GetBufferPointer();
		return code;
	}
    
	ShaderCompiler::ShaderCompiler()
	{
		DXCall(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&this->m_compiler)));
		DXCall(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&this->m_library)));
        DXCall(this->m_library->CreateIncludeHandler(&m_include_handler));
        
	}
}

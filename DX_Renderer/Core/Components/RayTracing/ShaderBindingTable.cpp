#include "ShaderBindingTable.hpp"
#include "../Shader/RayTracingShaders/RayGenShader.hpp"
#include "../Shader/RayTracingShaders/MissShader.hpp"
#include "../GraphicsDevice.hpp"

namespace DXR
{
	RayGenSBTEntry::RayGenSBTEntry(RayGenShader& Shader)
	{
		this->shader = shader;
	}

	void RayGenSBTEntry::AddResource(D3D12_GPU_DESCRIPTOR_HANDLE& Handle)
	{
		this->resources.push_back(Handle);
	}

	UINT RayGenSBTEntry::CalculateEntrySize()
	{
		UINT shader_id_size = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		UINT resource_count = this->resources.size();
		UINT resource_size = resource_count * 8;
		return Align(shader_id_size+resource_size,16);
	}

	MissSBTEntry::MissSBTEntry(RayGenShader& Shader)
	{
		this->shader = shader;
	}
	
	void MissSBTEntry::AddResource(D3D12_GPU_DESCRIPTOR_HANDLE& Handle)
	{
		this->resources.push_back(Handle);
	}
	
	UINT MissSBTEntry::CalculateEntrySize()
	{
		UINT shader_id_size = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		UINT resource_count = this->resources.size();
		UINT resource_size = resource_count * 8;
		return Align(shader_id_size + resource_size, 16);
	}
}
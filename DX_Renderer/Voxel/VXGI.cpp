#define NOMINMAX

#include <Windows.h>
#include <cmath>
#include "VXGI.hpp"
#include "../Core/Components/GraphicsDevice.hpp"
#include "../Core/Components/Command Queue/GraphicsCommandQueue.hpp"
#include "../Tooling/Log.hpp"

namespace DXR
{
	SceneVoxelizer::SceneVoxelizer(GraphicsDevice& Device)
	{
		this->CreateRendererInterface(Device);
        this->CreateVXGIObjects();
	}
    
	void SceneVoxelizer::CreateRendererInterface(GraphicsDevice& Device)
	{
		this->renderer_interface  = new NVRHI::RendererInterfaceD3D12(&error_handler,Device.GetRawInterface(),Device.GetGraphicsCommandQueue().GetCommandQueueRawPtr());
	}
    
	void SceneVoxelizer::CreateVXGIObjects()
	{
        VXGI::GIParameters parameters = {};
        parameters.rendererInterface = this->renderer_interface;
        parameters.errorCallback = &this->error_handler;
        
        VXGI::ShaderCompilerParameters compiler_parameters = {};
        compiler_parameters.errorCallback = &this->error_handler;
        compiler_parameters.graphicsAPI = this->renderer_interface->getGraphicsAPI();
        compiler_parameters.d3dCompilerDLLName = "d3dcompiler_47.dll";
        
        if(VXGI_FAILED(VFX_VXGI_CreateShaderCompiler(compiler_parameters,&this->compiler)))
        {
            ERROR_LOG(L"Failed To Create VXGI Shader Compiler");
            exit(-1);
        }
        
        if(VXGI_FAILED(VFX_VXGI_CreateGIObject(parameters,&this->gi)))
        {
            ERROR_LOG(L"Failed To Create VXGI Global Illumination Object");
            exit(-1);
        }
        
        if(VXGI_FAILED(this->gi->createBasicTracer(&this->tracer,this->compiler)))
        {
            ERROR_LOG(L"Failed To Create VXGI Tracer Object");
            exit(-1);
        }
        
        VXGI::VoxelizationParameters voxel_parameters = {};
        voxel_parameters.ambientOcclusionMode = false;
        voxel_parameters.mapSize = VXGI::uint3(128);
        
        if(VXGI_FAILED(this->gi->setVoxelizationParameters(voxel_parameters)))
        {
            ERROR_LOG(L"Failed To Set Voxelization Parameters");
            exit(-1);
        }
        
        return;
	}
    
	void ErrorCallbackHandler::signalError(const char* file, int line, const char* errorDesc)
	{
		//TODO(Tiago): Make this your own, right now its just copied and shit
        
		char buffer[4096];
		int length = (int)strlen(errorDesc);
		length = std::min(length, 4000); // avoid a "buffer too small" exception for really long error messages
		sprintf_s(buffer, "%s:%i\n%.*s", file, line, length, errorDesc);
        
		OutputDebugStringA(buffer);
		OutputDebugStringA("\n");
		MessageBoxA(NULL, buffer, "ERROR", MB_ICONERROR | MB_OK);
	}
    
}
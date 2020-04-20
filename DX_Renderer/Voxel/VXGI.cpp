#define NOMINMAX

#include <Windows.h>
#include <cmath>
#include <locale>
#include <codecvt>
#include <fstream>
#include <sstream>

#include "VXGI.hpp"
#include "../Core/Components/GraphicsDevice.hpp"
#include "../Core/Components/Command Queue/GraphicsCommandQueue.hpp"
#include "../Tooling/Log.hpp"

#include "../Resources/Shaders/reflection.h"
#include "../Camera/Camera.hpp"

namespace DXR
{
	SceneVoxelizer::SceneVoxelizer(GraphicsDevice& Device,VertexShader& vertex_shader, PixelShader& pixel_shader)
	{
        //TODO(Tiago): I dont really need to store these, I can just pass them to the functions that need them
        this->normal_vertex_shader = &vertex_shader;
        this->normal_pixel_shader = &pixel_shader;
        
		this->CreateRendererInterface(Device);
        this->CreateVXGIObjects();
        this->CreateVoxelizationGeometryShader();
        this->CreateVoxelizationPixelShader();
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
        voxel_parameters.mapSize = VXGI::uint3(64);
        
        if(VXGI_FAILED(this->gi->setVoxelizationParameters(voxel_parameters)))
        {
            ERROR_LOG(L"Failed To Set Voxelization Parameters");
            exit(-1);
        }
        
        return;
	}
    
    void SceneVoxelizer::CreateVoxelizationGeometryShader()
    {
        VXGI::IBlob* gs_blob = nullptr;
        
        if(VXGI_FAILED(compiler->compileVoxelizationGeometryShaderFromVS(&gs_blob,
                                                                         g_VSMain,sizeof(g_VSMain))))
        {
            __debugbreak();
            exit(-1);
        }
        if(VXGI_FAILED(gi->loadUserDefinedShaderSet(&voxelization_geometry_shader, gs_blob->getData(), gs_blob->getSize())))
        {
            __debugbreak();
            exit(-1);
        }
        gs_blob->dispose();
    }
    
    void SceneVoxelizer::CreateVoxelizationPixelShader()
    {
        //TODO(Tiago): Clean UP THIS MESS!
		std::wstring string_to_convert = this->normal_pixel_shader->filepath.c_str();
        
		//setup converter
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;
        
		//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
		std::string converted_str = converter.to_bytes(string_to_convert);
        
        std::stringstream builder;
        std::string line;
		std::ifstream myfile(converted_str);
		if (myfile.is_open())
		{
			while (std::getline(myfile, line))
			{
				builder << line;
			}
			myfile.close();
            line = builder.str();
		}
        
        VXGI::ShaderResources resources;
        resources.constantBufferCount = 1;
        resources.constantBufferSlots[0] = 0;
        resources.textureCount = 1;
        resources.textureSlots[0] = 0;
        resources.samplerCount = 1;
        resources.samplerSlots[0] = 0;
        
        VXGI::IBlob* blob = nullptr;
        
        //TODO(Tiago): Even if this works, the shader i gave it does not work as it requests so it might create issue, as an alternative i can use the default shader is has for the pixel shader as i really dont care to do anything there rn
        /*
        if(VXGI_FAILED(compiler->compileVoxelizationPixelShader(&blob,line.c_str(),line.size(),"PSMain", resources)))
        {
            __debugbreak();
            exit(-1);
        }
        */
        if(VXGI_FAILED(compiler->compileVoxelizationDefaultPixelShader(&blob)))
        {
            __debugbreak();
            exit(-1);
        }
        if(VXGI_FAILED(gi->loadUserDefinedShaderSet(&voxelization_pixel_shader, blob->getData(), blob->getSize())))
        {
            __debugbreak();
            exit(-1);
        }
    }
    
    void SceneVoxelizer::Voxelize(Camera& camera)
    {
        XMVECTOR anchor = {
            camera.position.x + camera.view_direction.x * 8,
            camera.position.y + camera.view_direction.y * 8,
            camera.position.z + camera.view_direction.z * 8,
            1.0f
        };
        VXGI::UpdateVoxelizationParameters params;
        params.clipmapAnchor = VXGI::float3(anchor.m128_f32);
        params.finestVoxelSize = 8;

        bool use_opacity = false;
        bool use_emitance = false;
        gi->prepareForVoxelization(params,use_opacity,use_emitance);

        if(use_emitance || use_opacity)
        {

            VXGI::VoxelizationViewParameters view_params;
            gi->getVoxelizationViewParameters(view_params);

            DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, 1280.0f / 720.0f, 0.1f, 1000.0f);
            VXGI::float4x4 voxelization_matrix = view_params.viewMatrix * view_params.projectionMatrix;

			const uint32_t maxRegions = 128;
			uint32_t numRegions = 0;
			VXGI::Box3f regions[maxRegions];

			if (VXGI_SUCCEEDED(gi->getInvalidatedRegions(regions, maxRegions, numRegions)))
			{
				NVRHI::DrawCallState emptyState;
				//g_pSceneRenderer->RenderForVoxelization(emptyState, g_pGI, regions, numRegions, voxelizationMatrix, NULL);
			}

        }

        gi->finalizeVoxelization();
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
#pragma once

#include "../ThirdParty/VXGI/GFSDK_VXGI.h"
#include "../ThirdParty/VXGI/GFSDK_NVRHI_D3D12.h"
#include "../Core/Components/Shader/VertexShader.hpp"
#include "../Core/Components/Shader/PixelShader.hpp"

namespace DXR
{
    
	struct GraphicsDevice;
    struct Camera;

	struct ErrorCallbackHandler:public NVRHI::IErrorCallback
	{
		void signalError(const char* file, int line, const char* errorDesc);
	};
    
	struct SceneVoxelizer
	{
        public:
		ErrorCallbackHandler error_handler;
		NVRHI::RendererInterfaceD3D12* renderer_interface = nullptr;
        VXGI::IGlobalIllumination* gi = nullptr;
        VXGI::IBasicViewTracer* tracer = nullptr;
        VXGI::IShaderCompiler* compiler = nullptr;
        
        //TODO(Tiago): Improve this such that they dont become dangling pointers
        VertexShader* normal_vertex_shader = nullptr;//NOTE(Tiago): this is a non owning pointer aka a reference
        PixelShader* normal_pixel_shader = nullptr;
        VXGI::IUserDefinedShaderSet* voxelization_geometry_shader = nullptr;//NOTE(Tiago): we do own this pointers lifetime
        VXGI::IUserDefinedShaderSet* voxelization_pixel_shader = nullptr;
        public:
		SceneVoxelizer() = default;
		SceneVoxelizer(GraphicsDevice& Device,VertexShader& vertex_shader, PixelShader& pixel_shader);
        void Voxelize(Camera& camera);
        private:
		void CreateRendererInterface(GraphicsDevice& Device);
		void CreateVXGIObjects();
        void CreateVoxelizationGeometryShader();
        void CreateVoxelizationPixelShader();
	};
    
}
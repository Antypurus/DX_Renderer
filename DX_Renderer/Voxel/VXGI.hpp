#pragma once

#include "../ThirdParty/VXGI/GFSDK_VXGI.h"
#include "../ThirdParty/VXGI/GFSDK_NVRHI_D3D12.h"

namespace DXR
{
    
	struct GraphicsDevice;
    
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
        public:
		SceneVoxelizer() = default;
		SceneVoxelizer(GraphicsDevice& Device);
        private:
		void CreateRendererInterface(GraphicsDevice& Device);
		void CreateVXGIObjects();
	};
    
}
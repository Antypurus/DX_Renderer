#pragma once
#include "RootParameter.hpp"

namespace DXR
{

	enum class DescriptorType
	{
		CBV,
		UAV,
		SRV
	};
	
	struct DescriptorRootParameter:public RootParameter
	{
	public:
	private:
		UINT m_register_slot;
	public:
		DescriptorRootParameter(DescriptorType Type, UINT registerSlot,D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
	protected:
		D3D12_ROOT_PARAMETER GenerateRootParameter() override;
	};
}

#pragma once
#include "RootParameter.hpp"

namespace DXR
{

	struct ConstantRootParameter :public RootParameter
	{
	private:
		UINT m_register_slot;
		UINT m_number_of_values;
	public:
		ConstantRootParameter(UINT registerSlot, UINT numberOfValues, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
	protected:
		D3D12_ROOT_PARAMETER GenerateRootParameter() override;
	};

}

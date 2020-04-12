#include "ModelVertex.hpp"

namespace DXR
{
	OBJVertex::OBJVertex(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT2& uv, const XMFLOAT3& color)
	{
		this->position = position;
		this->normal = normal;
		this->uv = uv;
		this->color = color;
	}

	std::vector<D3D12_INPUT_ELEMENT_DESC> OBJVertex::GenerateInputElementDescription()
	{
		D3D12_INPUT_ELEMENT_DESC position_description = {};
		position_description.SemanticName = "POSITION";
		position_description.SemanticIndex = 0;
		position_description.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		position_description.InputSlot = 0;
		position_description.AlignedByteOffset = 0;
		position_description.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		position_description.InstanceDataStepRate = 0;

		D3D12_INPUT_ELEMENT_DESC uv_description = {};
		uv_description.SemanticName = "UV";
		uv_description.SemanticIndex = 0;
		uv_description.Format = DXGI_FORMAT_R32G32_FLOAT;
		uv_description.InputSlot = 0;
		uv_description.AlignedByteOffset = 12;
		uv_description.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		uv_description.InstanceDataStepRate = 0;

		D3D12_INPUT_ELEMENT_DESC normal_description = {};
		normal_description.SemanticName = "NORMAL";
		normal_description.SemanticIndex = 0;
		normal_description.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		normal_description.InputSlot = 0;
		normal_description.AlignedByteOffset = 20;
		normal_description.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		normal_description.InstanceDataStepRate = 0;

		D3D12_INPUT_ELEMENT_DESC color_description = {};
		color_description.SemanticName = "COLOR";
		color_description.SemanticIndex = 0;
		color_description.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		color_description.InputSlot = 0;
		color_description.AlignedByteOffset = 32;
		color_description.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		color_description.InstanceDataStepRate = 0;

		std::vector<D3D12_INPUT_ELEMENT_DESC> ret = {position_description,uv_description,normal_description,color_description};

		return ret;
	}

	UINT64 OBJVertex::GetElementSize() const
	{
		return sizeof(position) + sizeof(normal) + sizeof(uv) + sizeof(color);
	}

	void* OBJVertex::GetData()
	{
		return &this->position;
	}

	void OBJVertex::operator=(Vertex other)
	{
		//TODO(Tiago): someday we will get back to this :)
	}

	D3D12_INPUT_LAYOUT_DESC OBJVertex::GetInputLayout()
	{
		D3D12_INPUT_ELEMENT_DESC position_description = {};
		position_description.SemanticName = "POSITION";
		position_description.SemanticIndex = 0;
		position_description.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		position_description.InputSlot = 0;
		position_description.AlignedByteOffset = 0;
		position_description.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		position_description.InstanceDataStepRate = 0;

		D3D12_INPUT_ELEMENT_DESC uv_description = {};
		uv_description.SemanticName = "UV";
		uv_description.SemanticIndex = 0;
		uv_description.Format = DXGI_FORMAT_R32G32_FLOAT;
		uv_description.InputSlot = 0;
		uv_description.AlignedByteOffset = 12;
		uv_description.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		uv_description.InstanceDataStepRate = 0;

		D3D12_INPUT_ELEMENT_DESC normal_description = {};
		normal_description.SemanticName = "NORMAL";
		normal_description.SemanticIndex = 0;
		normal_description.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		normal_description.InputSlot = 0;
		normal_description.AlignedByteOffset = 20;
		normal_description.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		normal_description.InstanceDataStepRate = 0;

		D3D12_INPUT_ELEMENT_DESC color_description = {};
		color_description.SemanticName = "COLOR";
		color_description.SemanticIndex = 0;
		color_description.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		color_description.InputSlot = 0;
		color_description.AlignedByteOffset = 32;
		color_description.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		color_description.InstanceDataStepRate = 0;

		std::vector<D3D12_INPUT_ELEMENT_DESC>* input_elements_descriptions = new std::vector<D3D12_INPUT_ELEMENT_DESC>();
		input_elements_descriptions->push_back(position_description);
		input_elements_descriptions->push_back(uv_description);
		input_elements_descriptions->push_back(normal_description);
		input_elements_descriptions->push_back(color_description);

		D3D12_INPUT_LAYOUT_DESC input_layout = {};
		input_layout.NumElements = (UINT)(input_elements_descriptions->size());
		input_layout.pInputElementDescs = input_elements_descriptions->data();

		return input_layout;
	}

}
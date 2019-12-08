cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

struct VS_OUTPUT
{
	float4 position:POSITION;
};

VS_OUTPUT main(float4 vPos:POSITION)
{
	VS_OUTPUT output;

	output.position = mul(vPos,gWorldViewProj);

	return output;
}
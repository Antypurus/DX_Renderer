cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

struct VS_OUTPUT
{
	float4 position:SV_POSITION;
};

VS_OUTPUT main(float4 vPos:POSITION0)
{
	VS_OUTPUT output;

	//output.position = mul(gWorldViewProj,vPos);
	output.position = vPos;

	return output;
}
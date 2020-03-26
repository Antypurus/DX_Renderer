cbuffer MVPBuffer : register(b0)
{
	float4x4 MVP;
};

Texture2D gText: register(t1);
SamplerState gsampler: register(s1);

struct VS_OUTPUT
{
	float4 position:SV_POSITION;
	float2 uv:UV;
};

struct VS_INPUT
{
	float3 pos:POSITION;
	float2 uv:UV;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
	VS_OUTPUT output;

	output.position = mul(MVP,float4(input.pos, 1.0f));
	output.uv = input.uv;

	return output;
}

struct PS_OUTPUT
{
	float4 color:SV_TARGET;
};

PS_OUTPUT PSMain(VS_OUTPUT input)
{
	PS_OUTPUT output;

	output.color = gText.Sample(gsampler,input.uv);

	return output;
}
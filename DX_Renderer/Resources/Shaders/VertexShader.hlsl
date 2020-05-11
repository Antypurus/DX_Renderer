cbuffer MVPBuffer : register(b0)
{
	float4x4 MVP;
    float4x4 voxel_mat;
};

Texture2D gText: register(t0);
SamplerState gsampler: register(s0);
RWTexture3D<float4> irradiance_map : register(u0);
Texture3D irradiance_map_tex : register(t0,space1);

struct VS_OUTPUT
{
	float4 position:SV_POSITION;
	float2 uv:UV;
    float3 normal: NORMAL;
    float3 color : COLOR;
    float4 voxel_pos : VOX;
};

struct VS_INPUT
{
	float3 pos:POSITION;
	float2 uv:UV;
    float3 normal : NORMAL;
    float3 color : COLOR;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
	VS_OUTPUT output;

	output.position = mul(MVP,float4(input.pos, 1.0f));
	output.uv = input.uv;
    output.normal = input.normal;
	output.color = input.color;
	
    output.voxel_pos = mul(voxel_mat, float4(input.pos, 1.0f));

	return output;
}

struct PS_OUTPUT
{
	float4 color:SV_TARGET;
};

PS_OUTPUT PSMain(VS_OUTPUT input)
{
	PS_OUTPUT output;

    float3 vox = input.voxel_pos.xyz / input.voxel_pos.w - float3(1, 1, 1);
    int3 voxel = int3((input.voxel_pos.xyz / input.voxel_pos.w - float3(1, 1, 1)));
    float4 col = gText.Sample(gsampler, input.uv);
    float4 other_col = irradiance_map_tex.Sample(gsampler, vox / 128.0f);
    //float4 other_col = irradiance_map[voxel];

    output.color = 0.25 * col + 0.75 * other_col;
	return output;
}
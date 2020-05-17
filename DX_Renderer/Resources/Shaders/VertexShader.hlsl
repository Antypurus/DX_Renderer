cbuffer MVPBuffer : register(b0)
{
	float4x4 MVP;
    float4x4 VP;
    float4x4 voxel_mat;
};

cbuffer RTCBuffer:register(b0,space1)
{
    float4x4 voxel_space_matrix;
    float4 light_color;
    float3 light_position;
    float light_radius;
    float light_extent;
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
    float4 light_pos : LIG;
    float4 u_pos: POSR;
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
    output.light_pos = float4(light_position,1.0f);
    output.u_pos = float4(input.pos,1.0f);
    
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
    float distance = sqrt((input.u_pos.x-input.light_pos.x)*(input.u_pos.x-input.light_pos.x)+
                          (input.u_pos.y-input.light_pos.y)*(input.u_pos.y-input.light_pos.y)+
                          (input.u_pos.z-input.light_pos.z)*(input.u_pos.z-input.light_pos.z));
    float falloff = 1/abs(distance+0.1);
    float4 other_col = irradiance_map_tex.Sample(gsampler, float3(vox.x / 256.0f,vox.y / 256.0f,vox.z / 768.0f));
    //float4 other_col = irradiance_map[voxel];
    
    output.color = 0.25 * col + 0.75 * falloff * other_col * col;
	return output;
}
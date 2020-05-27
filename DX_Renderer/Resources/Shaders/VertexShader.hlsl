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
RWTexture3D<uint> irradiance_map : register(u0);
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
    output.normal = normalize(input.normal) * 0.5 + 0.5;
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

float4 RGBA8UintToFloat4(uint val)
{
    return float4(float((val & 0x000000FF)), float((val & 0x0000FF00) >> 8U), float((val & 0x00FF0000) >> 16U), float((val & 0xFF000000) >> 24U));
}

PS_OUTPUT PSMain(VS_OUTPUT input)
{
	PS_OUTPUT output;
    
    float3 vox = input.voxel_pos.xyz / input.voxel_pos.w;
    float3 fp_vox = vox - float3(1, 1, 1);
    int3 voxel = int3(vox) - int3(1, 1, 1);
    float4 col = gText.Sample(gsampler, input.uv);
    float distance = sqrt((input.u_pos.x-input.light_pos.x)*(input.u_pos.x-input.light_pos.x)+
                          (input.u_pos.y-input.light_pos.y)*(input.u_pos.y-input.light_pos.y)+
                          (input.u_pos.z-input.light_pos.z)*(input.u_pos.z-input.light_pos.z));
    float falloff = 1 / abs(distance + 0.1);
    float4 other_col = irradiance_map_tex.Sample(gsampler, float3(fp_vox.x / 128.0f, fp_vox.y / 128.0f, fp_vox.z / 128.0f));
    //uint irradiance_col = irradiance_map[voxel];
    //float4 other_col = RGBA8UintToFloat4(irradiance_col);
    //other_col.rgb *= other_col.a;
    //normalize(other_col);
    
    //output.color = 0.1 * col + 2 * falloff * other_col * col;
    output.color = other_col;
	return output;
}
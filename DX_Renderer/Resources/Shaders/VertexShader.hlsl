
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

float4 UnpackFloat4(uint val)
{
    uint r = (val & 0x000000FF);
    uint g = (val & 0x0000FF00) >> 8U;
    uint b = (val & 0x00FF0000) >> 16U;
    uint a = (val & 0xFF000000) >> 24U;
    return float4(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
}

float to_linear_space_component(float c)
{
    if(c <= 0.04045)
    {
        return c/12.92;
    }
    return pow(((c+0.055)/1.055),2.4);
}

float4 to_linear_space(float4 color)
{
    float r = to_linear_space_component(color.r);
    float g = to_linear_space_component(color.g);
    float b = to_linear_space_component(color.b);
    float a = to_linear_space_component(color.a);
    return float4(r,g,b,color.a);
}

float gamma_correct_component(float c)
{
    if( c <= 0.0031308)
    {
        return 12.92 * c;
    }
    return (pow(1.055*c,(1/2.4)) - 0.055);
}

float4 gamma_correct(float4 color)
{
    float r = gamma_correct_component(color.r);
    float g = gamma_correct_component(color.g);
    float b = gamma_correct_component(color.b);
    float a = gamma_correct_component(color.a);
    return float4(r,g,b,color.a);
}

float4 sample_voxel_grid(Texture3D texture, float3 voxel_position, float grid_resolution)
{
    float4 sample_sum = float4(0, 0, 0, 1);
    
    uint sample_count = 7;
    
    float4 samples[7];
    samples[0] = (texture.Sample(gsampler, float3( voxel_position / grid_resolution)));
    samples[1] = (texture.Sample(gsampler, float3((voxel_position + float3( 1.0, 0, 0)) / grid_resolution)));
    samples[2] = (texture.Sample(gsampler, float3((voxel_position + float3(-1.0, 0, 0)) / grid_resolution)));
    samples[3] = (texture.Sample(gsampler, float3((voxel_position + float3( 0, 1.0, 0)) / grid_resolution)));
    samples[4] = (texture.Sample(gsampler, float3((voxel_position + float3( 0,-1.0, 0)) / grid_resolution)));
    samples[5] = (texture.Sample(gsampler, float3((voxel_position + float3( 0, 0, 1.0)) / grid_resolution)));
    samples[6] = (texture.Sample(gsampler, float3((voxel_position + float3( 0, 0,-1.0)) / grid_resolution)));
    
    for (uint i = 0; i < sample_count;++i)
    {
        sample_sum.rgb += samples[i].rgb;
    }
    
    float4 result = float4(sample_sum.rgb / sample_count, 1.0f);
    
    return (result);
}

PS_OUTPUT PSMain(VS_OUTPUT input)
{
	PS_OUTPUT output;
    
    float3 vox = input.voxel_pos.xyz/input.voxel_pos.w;
    float3 fp_vox = vox - float3(1, 1, 1);
    int3 voxel = int3(vox) - int3(1, 1, 1);
    
    float4 col = gText.Sample(gsampler, input.uv);
    
    float4 other_col = sample_voxel_grid(irradiance_map_tex, fp_vox, 128.0f);
    float4 result = 0.2 * col + 2 * other_col * col;
    result.a = 1;
    
    output.color = result;
	return output;
}
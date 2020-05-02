cbuffer ConversionMatrixBuffer : register(b0)
{
    float4x4 ClipSpaceMatrix;
    float4x4 VoxelSpaceMatrix;
};

RWTexture3D<float4> voxel_map : register(u0);
Texture2D gText : register(t0);
SamplerState gsampler : register(s0);

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 voxel_grip_position : VOXEL_POS;
    float2 uv : UV;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 color : COLOR;
};

VS_OUTPUT VoxelVSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.position = mul(ClipSpaceMatrix, float4(input.pos, 1.0f));
    output.voxel_grip_position = mul(VoxelSpaceMatrix, float4(input.pos, 1.0f));
    output.uv = input.uv;
    
    return output;
}

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

PS_OUTPUT VoxelPSMain(VS_OUTPUT input)
{
    PS_OUTPUT output;
    
    float3 gridPos = input.voxel_grip_position.xyz / input.voxel_grip_position.w;
    int3 voxel_pos = int3(gridPos);
    
    voxel_map[voxel_pos] = gText.Sample(gsampler, input.uv);
    
    output.color = voxel_map[voxel_pos];
    discard;
    
    return output;
}
cbuffer ConversionMatrixBuffer : register(b0)
{
    float4x4 ClipSpaceMatrix;
    float4x4 VoxelSpaceMatrix;
};

RWTexture3D<uint> voxel_map : register(u0);

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 voxel_grip_position : VOXEL_POS;
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
    
    return output;
}

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

PS_OUTPUT VoxelPSMain(VS_OUTPUT input)
{
    PS_OUTPUT output;

    output.color = float4(0, 0, 0, 1);
    
    int3 voxel_pos = int3(input.voxel_grip_position.xyz / input.voxel_grip_position.w);
    
    InterlockedOr(voxel_map[voxel_pos], 1 << (voxel_pos.z & 31));
    
    discard;
    return output;
}
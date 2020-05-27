cbuffer ConversionMatrixBuffer : register(b0)
{
    float4x4 ClipSpaceMatrix;
    float4x4 VoxelSpaceMatrix;
};

cbuffer MaterialProperties : register(b1)
{
    float4 ambient_coefficient;
    float4 diffuse_coefficient;
    float4 specular_coefficient;
    float4 specular_exponent;
}

RWTexture3D<uint> albedo_map : register(u0);
RWTexture3D<uint> ocupancy_map : register(u1);
RWTexture3D<uint> diffuse_map : register(u2);
RWTexture3D<uint> specular_map : register(u3);
RWTexture3D<uint> exponent_map : register(u4);
RWTexture3D<uint> normal_map : register(u5);

Texture2D gText : register(t0);
SamplerState gsampler : register(s0);

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 voxel_grip_position : VOXEL_POS;
    float4 normal : NORMAL;
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
    output.normal = float4(normalize(input.normal) * 0.5 + 0.5, 1.0f);
    
    return output;
}

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

// Packs float4 in [0,1] range into [0-255] uint
uint PackFloat4(float4 val)
{
    return (
        (uint(val.w * 255) & 0x000000FF) << 24U |
        (uint(val.z * 255) & 0x000000FF) << 16U |
        (uint(val.y * 255) & 0x000000FF) << 8U | 
        (uint(val.x * 255) & 0x000000FF));
}

// Unpacks values and returns float4 in [0,1] range
float4 UnpackFloat4(uint val)
{
    return float4(
            float((val & 0x000000FF)) / 255.0, 
            float((val & 0x0000FF00) >> 8U) / 255.0,
            float((val & 0x00FF0000) >> 16U) / 255.0,
            float((val & 0xFF000000) >> 24U) / 255.0);
}

void AverageRGBA8Voxel(RWTexture3D<uint> voxel_map, int3 voxel_coords, float4 val)
{
    uint packed_color = PackFloat4(float4(val.rgb, 1.0f/255.0f));
    uint previousStoredValue = 0;
    uint currentStoredValue;
    
    float4 currValue;
    float3 average;
    uint count;
    
    InterlockedCompareExchange(voxel_map[voxel_coords], previousStoredValue, packed_color, currentStoredValue);
    while (currentStoredValue != previousStoredValue)
    {
        previousStoredValue = currentStoredValue;
        currValue = UnpackFloat4(previousStoredValue);
        
        average = currValue.rgb;
        count = uint(currValue.a * 255.0f);
        
        average = (average * count + val.rgb) / (count + 1);
        
        packed_color = PackFloat4(float4(average, (count + 1)/255.0f));
        InterlockedCompareExchange(voxel_map[voxel_coords], previousStoredValue, packed_color, currentStoredValue);
    }
}

PS_OUTPUT VoxelPSMain(VS_OUTPUT input)
{
    PS_OUTPUT output;
    
    float3 gridPos = input.voxel_grip_position.xyz / input.voxel_grip_position.w;
    int3 voxel_pos = int3(gridPos.x - 1, gridPos.y - 1, gridPos.z - 1);
    
    ocupancy_map[voxel_pos] = uint(1);
    float4 frag_color = gText.Sample(gsampler, input.uv);
    AverageRGBA8Voxel(albedo_map, voxel_pos, frag_color);
    AverageRGBA8Voxel(diffuse_map, voxel_pos, normalize(diffuse_coefficient));
    AverageRGBA8Voxel(specular_map, voxel_pos, specular_coefficient);
    AverageRGBA8Voxel(normal_map, voxel_pos, input.normal);
    output.color = input.voxel_grip_position / 256.0f;
    discard;
    
    return output;
}
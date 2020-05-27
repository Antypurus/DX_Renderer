
struct BuiltinIntersectionAttribs
{ // Barycentric coordinates of hit in
    float2 barycentrics; // the triangle are: (1-x-y, x, y)
};

cbuffer RTCBuffer : register(b0)
{
    float4x4 voxel_space_matrix;
    float4 light_color;
    float3 light_position;
    float light_radius;
    float light_extent;
};

RaytracingAccelerationStructure Scene : register(t1);
RWTexture3D<uint> RenderTarget : register(u0);
RWTexture3D<uint> normal_map : register(u1);

uint Float4ToRGBA8Uint(float4 val)
{
    return (uint(val.w * 255.0) & 0x000000FF) << 24U | (uint(val.z * 255.0) & 0x000000FF) << 16U | (uint(val.y * 255.0) & 0x000000FF) << 8U | (uint(val.x * 255.0) & 0x000000FF);
}

float4 RGBA8UintToFloat4(uint val)
{
    return float4(float((val & 0x000000FF))/255.0, float((val & 0x0000FF00) >> 8U)/255.0, float((val & 0x00FF0000) >> 16U)/255.0, float((val & 0xFF000000) >> 24U)/255.0);
}

void AverageRGBA8Voxel(RWTexture3D<uint> voxel_map, int3 voxel_coords, float4 val)
{
    //val.rgb *= 255.0f;
    uint packed_color = Float4ToRGBA8Uint(float4(val.rgb, 1.0f));
    uint previousStoredValue = 0;
    uint currentStoredValue;
    
    float4 currValue;
    float3 average;
    uint count;
    
    InterlockedCompareExchange(voxel_map[voxel_coords], previousStoredValue, packed_color, currentStoredValue);
    while (currentStoredValue != previousStoredValue)
    {
        previousStoredValue = currentStoredValue;
        currValue = RGBA8UintToFloat4(previousStoredValue);
        
        average = currValue.rgb;
        count = currValue.a;
        
        average = (average * count + val.rgb) / (count + 1);
        
        packed_color = Float4ToRGBA8Uint(float4(average, (count + 1)));
        InterlockedCompareExchange(voxel_map[voxel_coords], previousStoredValue, packed_color, currentStoredValue);
    }
}

struct RayPayload
{
    float4 color;
    float3 origin;
    float3 direction;
};

[shader("raygeneration")]
void raygen()
{
    // Initialize the ray payload
    RayPayload payload;
    payload.color = light_color;
    payload.origin = float3(0, 0, 0);
    payload.direction = float3(0, 0, 0);
    
    // Get the location within the dispatched 2D grid of work items
    // (often maps to pixels, so this could represent a pixel coordinate).
    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (((launchIndex.xy + 0.5f) / dims.xy) * 2.f - 1.f);
    
    float3 direction = float3(DispatchRaysIndex().xyz) - float3(256, 256, 256);
    float4 origin = mul(voxel_space_matrix, float4(light_position, 1.0f));
    
    //ray.Origin = (origin.xyz / origin.w) - float3(1, 1, 1);
    RayDesc ray;
    ray.Origin = light_position;
    ray.Direction = direction;
    ray.TMin = 0.0;
    ray.TMax = 100000;
    
    TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 0xFF, 0, 0, 0, ray, payload);
    {
        //Shade Primary Hit
        float4 hit_pos = float4(payload.origin, 1.0f);
        hit_pos = mul(voxel_space_matrix, hit_pos);
        hit_pos.rgb /= hit_pos.w;
        int3 map_pos = int3(hit_pos.x - 1, hit_pos.y - 1, hit_pos.z - 1);
        //int3 map_pos = int3(hit_pos.x, hit_pos.y, hit_pos.z);
        uint packed_normal = normal_map[map_pos];
        float4 normal = RGBA8UintToFloat4(packed_normal);
        //normal.rgb = (normal.rgb - 0.5) / 0.5;
        float falloff = saturate(dot(normalize(ray.Direction), -normal.rgb));
        float4 final_irradiance = float4(falloff * light_color.rgb * light_color.a, 1.0f);
        AverageRGBA8Voxel(RenderTarget, map_pos, final_irradiance);
    }
    
    ray.Origin = payload.origin;
    ray.Direction = payload.direction;
    //TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 0xFF, 0, 0, 0, ray, payload);
}

[shader("intersection")]
void intersection()
{
}

[shader("miss")]
void miss(inout RayPayload data : SV_RayPayload)
{
    data.color = float4(0, 0, 0.0f, 0.0f);
}

[shader("anyhit")]
void anyhit(inout RayPayload data, BuiltinIntersectionAttribs hit)
{
    data.color = float4(1.0f, 0, 0.0f, 1.0f);
}

[shader("closesthit")]
void closesthit(inout RayPayload data, in BuiltinIntersectionAttribs hit)
{
    float dist = RayTCurrent();
    float3 ray_origin = WorldRayOrigin();
    float3 ray_dir = WorldRayDirection();
    
    float4 hit_position = float4(ray_origin + mul(ray_dir, dist), 1.0f);
    float4 hit_pos = hit_position;
    hit_pos = mul(voxel_space_matrix, hit_pos);
    hit_pos.rgb /= hit_pos.w;
    int3 map_pos = int3(hit_pos.x - 1, hit_pos.y - 1, hit_pos.z - 1);
    uint packed_normal = normal_map[map_pos];
    float4 normal = RGBA8UintToFloat4(packed_normal) / 256;
   
    float3 new_dir = reflect(normalize(ray_dir), normal.rgb);
    data.color = float4(1, 0, 0, 1);
    data.direction = new_dir;
    data.origin = hit_position.rgb;
}
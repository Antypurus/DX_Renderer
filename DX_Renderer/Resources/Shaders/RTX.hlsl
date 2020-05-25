
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
RWTexture3D<float4> RenderTarget : register(u0);
RWTexture3D<uint> normal_map : register(u0, space1);

float nrand(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

struct RayPayload
{
    float4 color;
};

[shader("raygeneration")]
void raygen()
{
    // Initialize the ray payload
    RayPayload payload;
    payload.color = float4(0, 0, 0, 0);
    
    // Get the location within the dispatched 2D grid of work items
    // (often maps to pixels, so this could represent a pixel coordinate).
    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (((launchIndex.xy + 0.5f) / dims.xy) * 2.f - 1.f);
    
    RayDesc ray;
    float3 direction = float3(DispatchRaysIndex().xyz) - float3(256, 256, 256);
    float4 origin = mul(voxel_space_matrix, float4(light_position, 1.0f));
    
    //ray.Origin = (origin.xyz / origin.w) - float3(1, 1, 1);
    ray.Origin = light_position;
    ray.Direction = direction;
    ray.TMin = 0;
    ray.TMax = 100000;
    
    TraceRay(Scene, RAY_FLAG_NONE, 0xFF, 0, 0, 0, ray, payload);
    
    int3 map_pos = int3(0, 1, 0);
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

float4 RGBA8UintToFloat4(uint val)
{
    return float4(float((val & 0x000000FF)), float((val & 0x0000FF00) >> 8U), float((val & 0x00FF0000) >> 16U), float((val & 0xFF000000) >> 24U));
}

[shader("closesthit")]
void closesthit(inout RayPayload data, BuiltinIntersectionAttribs hit)
{
    float dist = RayTCurrent();
    float3 ray_origin = WorldRayOrigin();
    float3 ray_dir = WorldRayDirection();
    
    float3 hit_pos = ray_origin + mul(ray_dir, dist);
    hit_pos = mul(voxel_space_matrix, float4(hit_pos, 1.0f)).xyz;
    int3 map_pos = int3(hit_pos.x - 1, hit_pos.y - 1, hit_pos.z - 1);
    //int3 map_pos = int3(hit_pos.x, hit_pos.y, hit_pos.z);
    uint packed_normal = normal_map[map_pos];
    float4 normal = RGBA8UintToFloat4(packed_normal)/256;
    float falloff = 1;
    //dot(ray_dir, normal.rgb);
    RenderTarget[map_pos] = falloff * float4(light_color);
    RenderTarget[map_pos + int3(1, 0, 0)] = falloff * float4(light_color);
    RenderTarget[map_pos + int3(-1, 0, 0)] = falloff * float4(light_color);
    RenderTarget[map_pos + int3(0, 1, 0)] = falloff * float4(light_color);
    RenderTarget[map_pos + int3(0, -1, 0)] = falloff * float4(light_color);
    RenderTarget[map_pos + int3(0, 0, 1)] = falloff * float4(light_color);
    RenderTarget[map_pos + int3(0, 0, -1)] = falloff * float4(light_color);
    if(normal.w != 0)
    {
        float3 new_dir = reflect(ray_dir, normal.rgb);
        RayDesc ray;
        float3 direction = new_dir;
        float3 origin = hit_pos;
        ray.Origin = origin;
        ray.Direction = direction;
        ray.TMin = 0;
        ray.TMax = 100000;
        TraceRay(Scene, RAY_FLAG_NONE, 0xFF, 0, 0, 0, ray, data);
    }
    
    data.color = float4(1.0f, 0, 0.0f, 1.0f);
    
}
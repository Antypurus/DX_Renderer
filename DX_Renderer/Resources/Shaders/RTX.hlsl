
struct BuiltinIntersectionAttribs
{ // Barycentric coordinates of hit in
    float2 barycentrics; // the triangle are: (1-x-y, x, y)
};

cbuffer RTCBuffer:register(b0)
{
    float4x4 voxel_space_matrix;
    float3 light_position;
    float2 ray_angle_delta;
};

RaytracingAccelerationStructure Scene : register(t1);
RWTexture3D<float4> RenderTarget : register(u0);

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

    for (float h_angle = 0.0f; h_angle < 360.0f; h_angle += ray_angle_delta.x)
    {
        for (float v_angle = 0.0f; v_angle < 360.0f; v_angle += ray_angle_delta.y)
        {
            RayDesc ray;
            float x = cos(0);
            float y = sin(0);
            float z = sin(0);
            float3 direction = float3(x, y, z);
            
            ray.Origin = mul(voxel_space_matrix, float4(light_position,1.0f)).xyz;
            ray.Direction = direction;
            ray.TMin = 0;
            ray.TMax = 100000;
            
            TraceRay(Scene, RAY_FLAG_NONE, 0xFF, 0, 0, 0, ray, payload);
        }
    }
    //RenderTarget[DispatchRaysIndex().xy] = payload.color;
}

[shader("intersection")]
void intersection()
{
}

[shader("miss")]
void miss(inout RayPayload data : SV_RayPayload)
{
    data.color = float4(0, 0, 1.0f,1.0f);
}

[shader("anyhit")]
void anyhit(inout RayPayload data, BuiltinIntersectionAttribs hit)
{
    data.color = float4(1.0f, 0, 0.0f, 1.0f);
}

[shader("closesthit")]
void closesthit(inout RayPayload data, BuiltinIntersectionAttribs hit)
{
    float dist = RayTCurrent();
    float3 ray_origin = WorldRayOrigin();
    float3 ray_dir = WorldRayDirection();
    
    float3 hit_pos = ray_origin + mul(ray_dir, dist);
    int3 map_pos = int3(hit_pos);
    RenderTarget[map_pos] = float4(1, 1, 1, 1);
    data.color = float4(1.0f, 0, 0.0f, 1.0f);
}
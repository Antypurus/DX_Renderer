
struct BuiltinIntersectionAttribs
{ // Barycentric coordinates of hit in
    float2 barycentrics; // the triangle are: (1-x-y, x, y)
};

RaytracingAccelerationStructure Scene : register(t1);
RWTexture2D<float4> RenderTarget : register(u0);

struct RayPayload
{
    float4 color;
};

[shader("raygeneration")]
void raygen()
{
    float3 rayDir = float3(0, 0, 1);
    float3 rayOrigin = float3(640.0f, 360.0f, 0.0f);
    
    RayDesc ray;
    ray.Origin = rayOrigin;
    ray.Direction = rayDir;
    ray.TMin = 0.001;
    ray.TMax = 10000.0;
    RayPayload payload = { float4(0, 0, 0, 0) };
    TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, ~0, 0, 1, 0, ray, payload);
    
    RenderTarget[DispatchRaysIndex().xy] = payload.color;
}

[shader("intersection")]
void intersection()
{
}

[shader("miss")]
void miss(inout RayPayload data)
{
    data.color = float4(0, 0, 1.0f,1.0f);
}

[shader("anyhit")]
void anyhit(inout RayPayload data, BuiltinIntersectionAttribs attribs)
{
    data.color = float4(1.0f, 0, 0.0f, 1.0f);
}

[shader("closesthit")]
void closesthit(inout RayPayload data, BuiltinIntersectionAttribs attribs)
{
}
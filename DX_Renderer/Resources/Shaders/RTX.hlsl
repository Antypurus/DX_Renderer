
struct BuiltinIntersectionAttribs
{ // Barycentric coordinates of hit in
    float2 barycentrics; // the triangle are: (1-x-y, x, y)
};


struct RayPayload
{
    float3 color;
};

[shader("raygeneration")]
void raygen()
{
}

[shader("intersection")]
void intersection()
{
}

[shader("miss")]
void miss(inout RayPayload data)
{
    data.color = float3(0, 0, 1.0f);
}

[shader("anyhit")]
void anyhit(inout RayPayload data, BuiltinIntersectionAttribs attribs)
{
    data.color = float3(1.0f, 0, 0.0f);
}

[shader("closesthit")]
void closesthit(inout RayPayload data, BuiltinIntersectionAttribs attribs)
{
}
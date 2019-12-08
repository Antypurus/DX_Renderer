cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

void VS(float3 iPosL : POSITION,
		out float4 oPosH : SV_POSITION,
		out float4 oColor : COLOR)
{
	// Transform to homogeneous clip space.
	oPosH = mul(float4(iPosL, 1.0f), gWorldViewProj);

	// Just pass vertex color into the pixel shader.
	oColor = float4(1.0f.0.0f,0.0f,0.0f);
}
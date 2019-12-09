struct VS_OUTPUT
{
	float4 position:SV_POSITION;
};

VS_OUTPUT VSMain(float3 vPos:POSITION)
{
	VS_OUTPUT output;

	//output.position = mul(gWorldViewProj,vPos);
	output.position = float4(vPos,1.0f);

	return output;
}

struct PS_OUTPUT
{
	float4 color:SV_TARGET;
};

PS_OUTPUT PSMain(VS_OUTPUT input)
{
	PS_OUTPUT output;
	output.color = float4(1.0f,0.0f,0.0f,1.0f);

	return output;
}
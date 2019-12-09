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
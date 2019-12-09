struct PS_OUTPUT
{
	float4 color:SV_TARGET;
};

struct VS_OUTPUT
{
	float4 position:SV_POSITION;
};

PS_OUTPUT PSMain(VS_OUTPUT input)
{
	PS_OUTPUT output;
	output.color = float4(1.0f,1.0f,1.0f,1.0f);

	return output;
}
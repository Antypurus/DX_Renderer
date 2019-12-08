struct PS_OUTPUT
{
	float4 color:SV_TARGET;
};

PS_OUTPUT main()
{
	PS_OUTPUT output;
	output.color = float4(1.0f,0.0f,0.0f,1.0f);

	return output;
}

struct VS_OUTPUT
{
	float4 pos:POSITION;
};

VS_OUTPUT main() // main is the default function name
{
	VS_OUTPUT Output; 
	Output.pos = float4(0.0f,0.0f,0.0f,0.0f);
    return Output;
}
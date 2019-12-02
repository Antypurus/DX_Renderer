
struct VS_INPUT
{
    float3 vPos   : POSITION;
    float3 vColor : COLOR0;
};

float3 main(VS_INPUT input) // main is the default function name
{
    float3 Output = float3(0.0f,0.0f,0.0f);
    return Output;
}
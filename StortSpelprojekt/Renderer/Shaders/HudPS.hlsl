
struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

Texture2D tex : register(t0);
SamplerState ObjSamplerState : register(s0);

float4 main(VS_OUT input) : SV_TARGET
{
	return float4(tex.Sample(ObjSamplerState, input.uv).xyz, 1.0f);
}
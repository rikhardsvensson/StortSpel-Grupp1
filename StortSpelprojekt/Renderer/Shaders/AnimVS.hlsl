/*--------------------------------------------------------------------------------------------------------------------
| Vertex shader that is used for rendering objects with skeletal animation								             |
--------------------------------------------------------------------------------------------------------------------*/

cbuffer matrixBufferPerFrame : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 ambientLight;
};

cbuffer matrixBufferPerObjectObject : register(b1)
{
	matrix worldMatrix;
	float3 colorOffset;
};

cbuffer matrixBufferPerObjectObject : register(b5)
{
	matrix bones[50];
};

struct VS_IN
{
	uint4 boneIndex		: BONEINDEX;
	float3 pos			: POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float4 boneWeight	: BONEWEIGHT;
};

struct VS_OUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 ambientLight : AMBIENT;
	float3 colorOffset  : COLOROFFSET;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	
	float4 inputPos = float4(input.pos, 1.0f);
	float4 animPos;

	// Calculating the new position for every vertex based on the bones matrices
	animPos = mul(inputPos, bones[input.boneIndex.y]) * input.boneWeight.y;
	animPos += mul(inputPos, bones[input.boneIndex.x]) * input.boneWeight.x;
	animPos += mul(inputPos, bones[input.boneIndex.z]) * input.boneWeight.z;
	animPos += mul(inputPos, bones[input.boneIndex.w]) * input.boneWeight.w;

	animPos = mul(animPos, worldMatrix);
	animPos = mul(animPos, viewMatrix);
	animPos = mul(animPos, projectionMatrix);

	output.pos = animPos;
	output.normal = input.normal;
	output.uv = input.uv;
	output.ambientLight = ambientLight;
	output.colorOffset = colorOffset;

	return output;
}
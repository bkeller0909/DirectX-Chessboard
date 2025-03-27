//
// include file for skybox shaders
//

cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	matrix worldMatrix;
	matrix worldViewProjectionMatrix;
	float4 worldCameraPos;	  // worldspace position of the camera
};

struct VS_INPUT
{
	float4 Pos : SV_POSITION;	// position
	float3 Normal: NORMAL;		// normal
	float2 UV : TEXCOORD0;		// texture coordinate
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 ViewDir : TEXCOORD0;
};
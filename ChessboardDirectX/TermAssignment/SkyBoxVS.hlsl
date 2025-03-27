//
// Skybox Vertex Shader
//

#include "SkyBox.hlsli"


PS_INPUT main( VS_INPUT input )
{
	PS_INPUT output;

	output.Pos = mul(input.Pos, worldViewProjectionMatrix);
	output.ViewDir = mul(input.Pos, worldMatrix).xyz - worldCameraPos.xyz;

	return output;
}
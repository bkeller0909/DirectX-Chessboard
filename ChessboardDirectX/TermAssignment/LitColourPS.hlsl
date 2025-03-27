 //
// Pixel shader for an unlit vertex colour shader
//

#include "VertexPositionNormalTexture.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
	// The interpolated color 
	//
	float3 normal = normalize(input.WorldNormal.xyz);


	// The interpolated color 
	//
	float4 texColor = MainTex.Sample(Sampler, input.UV);

	float4 finalColor =
		AmbientLighting(texColor, ambientColour)
		+ DiffuseLighting(directionalLightVector.xyz, normal, directionalLightColor, texColor)
		+ SpecularLightingBlinnPhong(directionalLightVector.xyz, normal, worldCameraPos.xyz, input.WorldPosition.xyz, specularLightColor, texColor, specularLightColor.a);
	//	+ DiffuseLighting(directionalLightVector2.xyz, normal, directionalLightColor2, texColor)
	//	+ SpecularLightingBlinnPhong(directionalLightVector2.xyz, normal, worldCameraPos.xyz, input.WorldPosition.xyz, specularLightColor2, texColor, specularLightColor2.a);


	finalColor.a = 1.0f;

	finalColor.a = 1.0f;
	return finalColor;
}
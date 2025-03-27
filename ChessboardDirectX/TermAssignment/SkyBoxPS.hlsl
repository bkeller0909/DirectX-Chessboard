//
// Skybox pixel shader
//

#include "SkyBox.hlsli"

TextureCube cubeTex : register(t0);
sampler		Sampler : register(s0);

float4 main( PS_INPUT input ) : SV_TARGET
{
	return cubeTex.Sample( Sampler, input.ViewDir );
}
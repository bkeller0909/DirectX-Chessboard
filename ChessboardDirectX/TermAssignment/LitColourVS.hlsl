// Vertex shader for an unlit vertex colour shader
//
//

#include "VertexPositionNormalTexture.hlsli"

PS_INPUT main( VS_INPUT input )
{
	PS_INPUT output;

	// transform the input position using world * view * projection
	//  by using the multiplication operator
	output.Pos = mul(input.Pos, worldViewProjectionMatrix);
	
	// set the color to be interpolated by the rasterizer
	output.Color = float4(1,1,1,1); 

	// transform the normal to world space but ignore the 
	output.WorldNormal = mul(float4(input.Normal, 0), worldMatrixIT).xyz;

	// we need the position in world space to do specular lighting
	output.WorldPosition = mul(input.Pos, worldMatrix).xyz;

	// copy the UVa
	output.UV = input.UV;

	return output;
}
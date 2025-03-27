
// Header file for using a vertex containing position and color
//
//
//

// constants that are needed by the shader
cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	matrix worldMatrix;					
	matrix viewMatrix;				
	matrix projectionMatrix;
	matrix worldViewProjectionMatrix;
	matrix worldMatrixIT; // inverse transpose of the worldMatrix
	float4 worldCameraPos;	  // worldspace position of the camera
};

// constants that are used to do lighting
cbuffer LIGHT_BUFFER : register(b1)
{
	float4 ambientColour;

	float4 directionalLightColor;
	float4 directionalLightVector;

	float4 specularLightColor; // use the alpha to store specular power

	float4 directionalLightColor2;
	float4 directionalLightVector2;
	float4 specularLightColor2; // use the alpha to store specular power

	float4 directionalLightColor3;
	float4 directionalLightVector3;
	float4 specularLightColor3; // use the alpha to store specular power

}

// represents a material
//
cbuffer MATERIAL_BUFFER: register(b2)
{
	float4 matAmbient;
	float4 matDiffuse;
	float4 matSpecular;
}

Texture2D MainTex : register(t0);
sampler Sampler : register(s0);


// vertex shader input
//  This is information that comes from the vertices
struct VS_INPUT
{
	float4 Pos : SV_POSITION;	// position
	float3 Normal: NORMAL;		// normal
	float2 UV : TEXCOORD0;		// texture coordinate
};

// pixel shader input
//  Information that is passed from vertex to pixel shader
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float3 WorldNormal : TEXCOORD1;
	float4 Color : COLOR;
	float3 WorldPosition : TEXCOORD2;
};


// function that will calculate ambient lighting
//
float4 AmbientLighting(float4 materialColour, float4 lightColour)
{
	return materialColour * lightColour;
}

// Given a light and normal, calculate the colour of the directional light contribution
//
float4 DiffuseLighting(float3 lightVector, float3 normal, float4 lightColor, float4 matColor)
{
	// Diffuse lighting calculate, take the dot product between the light and normal vectors
	//	this is the same as cosine of the angle between the vectors
	float intensity = max(0, dot(normal, lightVector));

	// multiply the material color by the light color, modulated by the 
	//  angle calculation
	return intensity * lightColor * matColor;

}
// Calculate the specular light using a blinn-phong specular model
//		(H * N) ^ power
//  
//
float4 SpecularLightingBlinnPhong(float3 lightVector, float3 normal, float3 cameraPos, float3 worldPos, float4 lightColor, float4 matColor, float specPower)
{
	// calculate the view vector
	float3 viewVector = normalize(cameraPos - worldPos);

	// calculate the half vector
	float3 halfVector = normalize(lightVector + viewVector);

	// take dot product of the half vector and normal to calculate the reflectance
	float spec = dot(halfVector, normal);

	// saturate clamps to 0 & 1, power 'tightens' the specularity
	spec = pow(saturate(spec), specPower);

	// calculate the final color
	return spec * matColor * lightColor;
}

// Phong specular model
//
float4 SpecularLightingPhong(float3 lightVector, float3 normal, float3 cameraPos, float3 worldPos, float4 lightColor, float4 matColor, float specPower)
{
	// calculate the view vector
	float3 viewVector = normalize(cameraPos - worldPos);

	// calculate the half vector
	float3 reflectVector = reflect(lightVector, normal);

	// take dot product of the reflection and view vectors
	float spec = dot(reflectVector, viewVector);

	// saturate clamps to 0 & 1, power 'tightens' the specularity
	spec = pow(saturate(spec), specPower);

	// calculate the final color
	return spec * matColor * lightColor;


}
//
// BGTD 9201 
//	Example of an unlit vertex coloured shader
//

#ifndef _LIT_COLOUR_SHADER_H
#define _LIT_COLOUR_SHADER_H


#include <d3d11_1.h>
#include <SimpleMath.h>



using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector4;
using DirectX::SimpleMath::Color;
using DirectX::SimpleMath::Vector3;

// aligns with the light constants
struct LightConstants
{
	Color  ambientLight;
	Color   directionalLightColor;
	Vector4 directionalLightVector;
	Color specularLightColor;

	Color   directionalLightColor2;
	Vector4 directionalLightVector2;
	Color specularLightColor2;

	Color   directionalLightColor3;
	Vector4 directionalLightVector3;
	Color specularLightColor3;

};

class LitColourShader
{
public:

	LitColourShader();
	~LitColourShader();

	// load and create the shader
	void LoadShader(ID3D11Device* pDevice);

	// get the information for the shader
	const void* GetVertexShaderBinary();
	size_t		GetVertexShaderBinarySize();

	// set the shaders
	void SetShaders(ID3D11DeviceContext* pContext, const Matrix& world, const Matrix& view, const Matrix& projection);

	// set the ambient light color
	void SetAmbientLight(Color clr); 
	
	// set the new directional light
	void SetDirectionalLight(Color clr, Vector3 direction);

	// set the specular light color and power
	void SetSpecularLight(Color clr, float power);

private:

	// data read from files
	ID3DBlob*			pVertexShaderBlob;
	ID3DBlob*			pPixelShaderBlob;

	// shaders
	ID3D11VertexShader* pVertexShader;
	ID3D11PixelShader*  pPixelShader;

	// constants
	ID3D11Buffer*		pConstantBuffer;
	ID3D11Buffer*		pLightsBuffer;

	ID3D11SamplerState*  pSampler;

	// lights
	LightConstants		lightingValues;

	// flag that tells us if the lights need to be updated
	bool				lightsDirty;


};


#endif
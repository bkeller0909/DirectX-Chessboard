//
// BTGD 9201 - Object that represents a skybox object
//

#ifndef _SKYBOX_H
#define _SKYBOX_H

#include <d3d11_1.h>
#include "TextureType.h"
#include "IndexedPrimitive.h"
#include <CommonStates.h>

class SkyBox
{
public:
	SkyBox();
	~SkyBox();

	// initialize the skybox with a texturename and size
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const wchar_t* textureName, int size);

	// draw the skybox
	void Draw(ID3D11DeviceContext* pDeviceContext, const Matrix& viewMatrix, const Matrix& projMatrix );

private:

	TextureType			texture;

	ID3D11PixelShader*	 pPixelShader;
	ID3D11VertexShader*  pVertexShader;

	IndexedPrimitive	 skyGeo;

	ID3D11Buffer*		 pConstants;
	ID3D11SamplerState*  pSamplerState;

	float scale;

	ID3DBlob *pVertexShaderBlob;
	ID3DBlob *pPixelShaderBlob;

	std::unique_ptr<CommonStates> states;

};

#endif
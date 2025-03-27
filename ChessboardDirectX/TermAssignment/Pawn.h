//
// Model of a pawn
//
//  BGTD 9201
//



#ifndef _PAWN_H
#define _PAWN_H

#include "DirectX.h"
#include "IndexedPrimitive.h"
#include "LitColourShader.h"
#include <d3d11_1.h>
#include <SimpleMath.h>

using namespace DirectX;

class Pawn
{
public:

	Pawn();
	~Pawn();

	// called to initialize the object
	void Initialize(ID3D11Device* pDevice, LitColourShader* pLitShader, float baseOffset);

	// called to draw the object
	void Draw(ID3D11DeviceContext* pDeviceContext, const Matrix& parentMatrix, const Matrix& viewMatrix, const Matrix& projMatrix);

	// update the object
	void Update(float deltaTime);

	void SetTextures(ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* spec) {
		pDiffuse = diffuse;
		pSpec = spec;
	}

	void SetBaseOffset(const float inOffset) { baseOffset = inOffset; }
	float GetBaseOffset() { return baseOffset; }

private:

	ID3D11Buffer* MakeMaterialBuffer(ID3D11Device* pDevice, Color ambient, Color diffuse, Color spec, float specPower);

	ID3D11ShaderResourceView* pDiffuse;
	ID3D11ShaderResourceView* pSpec;

	// store a copy of the shader
	LitColourShader* pShader;

	// store each object
	IndexedPrimitive base;
	IndexedPrimitive middle;
	IndexedPrimitive top;

	Matrix baseMatrix;
	Matrix middleMatrix;
	Matrix topMatrix;

	ID3D11Buffer* pBaseBuffer;
	ID3D11Buffer* pMiddleBuffer;
	ID3D11Buffer* pTopBuffer;

	float baseOffset;
};

#endif
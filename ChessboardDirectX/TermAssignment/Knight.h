//
// Model of a Knight
//
//  BGTD 9201
//



#ifndef _Knight_H
#define _Knight_H

#include "DirectX.h"
#include "IndexedPrimitive.h"
#include "LitColourShader.h"
#include <d3d11_1.h>
#include <SimpleMath.h>

using namespace DirectX;

class Knight
{
public:

	Knight();
	~Knight();

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
	const static int NUM_TOP_PARTS = 2;
	const static int NUM_MIDDLE_PARTS = 3;
	const static int NUM_BOTTOM_PARTS = 1;

	IndexedPrimitive base[NUM_BOTTOM_PARTS];
	IndexedPrimitive middle[NUM_MIDDLE_PARTS];
	IndexedPrimitive top[NUM_TOP_PARTS];

	Matrix baseMatrix[NUM_BOTTOM_PARTS];
	Matrix middleMatrix[NUM_MIDDLE_PARTS];
	Matrix topMatrix[NUM_TOP_PARTS];

	ID3D11Buffer* pBaseBuffer;
	ID3D11Buffer* pMiddleBuffer;
	ID3D11Buffer* pTopBuffer;

	float baseOffset;
};

#endif
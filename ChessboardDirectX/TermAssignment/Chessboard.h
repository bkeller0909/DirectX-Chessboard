//
// Model of a Chessboard
//
//  BGTD 9201
//



#ifndef _Chessboard_H
#define _Chessboard_H

#include "DirectX.h"
#include "IndexedPrimitive.h"
#include "LitColourShader.h"
#include <d3d11_1.h>
#include <SimpleMath.h>

using namespace DirectX;

class Chessboard
{
public:

	Chessboard();
	~Chessboard();

	// called to initialize the object
	void Initialize(ID3D11Device* pDevice, LitColourShader* pLitShader, Matrix inWorldMatrix, Color colour1, Color colour2);

	// called to draw the object
	void Draw(ID3D11DeviceContext* pDeviceContext, const Matrix& parentMatrix, const Matrix& viewMatrix, const Matrix& projMatrix);

	// update the object
	void Update(float deltaTime);

	void SetTextures(ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* spec) {
		pDiffuse = diffuse;
		pSpec = spec;
	}

	Matrix GetBoardPosition(int x, int y, float pieceBaseOffset);

private:

	ID3D11Buffer* MakeMaterialBuffer(ID3D11Device* pDevice, Color ambient, Color diffuse, Color spec, float specPower);

	ID3D11ShaderResourceView* pDiffuse;
	ID3D11ShaderResourceView* pSpec;

	// store a copy of the shader
	LitColourShader* pShader;

	// Chessboard
	const static int BOARDHEIHT_MODIFIER = 1;
	const static int X_LENGTH = 8;
	const static int Y_LENGTH = 8;
	float gridScale;

	IndexedPrimitive chessGrid[X_LENGTH][Y_LENGTH];
	Matrix chessGridMatrix[X_LENGTH][Y_LENGTH];

	Color gridColour1;
	Color gridColour2;

	Matrix worldPositionMatrix;

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


};

#endif
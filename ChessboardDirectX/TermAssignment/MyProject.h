#ifndef _MyProject_h
#define _MyProject_h

#include "DirectX.h"
#include "Font.h"
#include "TextureType.h"
#include "IndexedPrimitive.h"
#include "LitColourShader.h"
#include "Pawn.h"
#include "Bishop.h"
#include "Chessboard.h"
#include "King.h"
#include "Knight.h"
#include "Queen.h"
#include "Rook.h"
#include "SkyBox.h"

// forward declare the sprite batch

using namespace DirectX;
using namespace DirectX::SimpleMath;


//----------------------------------------------------------------------------------------------
// Main project class
//	Inherits the directx class to help us initalize directX
//----------------------------------------------------------------------------------------------

class MyProject : public DirectXClass
{
public:
	// constructor
	MyProject(HINSTANCE hInstance);
	~MyProject();

	// Initialize any directX objects
	void InitializeObjects();

	// window message handler
	LRESULT ProcessWindowMessages(UINT msg, WPARAM wParam, LPARAM lParam);

	// Called by the render loop to render a single frame
	void Render(void);

	// Called by directX framework to allow you to update any scene objects
	void Update(float deltaTime);

	// Compute the view and projection matrices
	void ComputeViewProjection();

private:

	LitColourShader shader;
	SkyBox skyBox;

	float runTime;

	// chessboard
	Chessboard chessboard;
	
	// player 1 chess pieces
	Color playerOneColour;	// white
	Pawn pawn;
	Bishop bishop;
	King king;
	Knight knight;
	Queen queen;
	Rook rook;

	// player 2 chess pieces
	Color playerTwoColour;	// black
	Pawn pawn2;
	Bishop bishop2;
	King king2;
	Knight knight2;
	Queen queen2;
	Rook rook2;

	// matrices
	Matrix pawnMatrix;
	Matrix rookMatrix;
	Matrix knightMatrix;
	Matrix bishopMatrix;
	Matrix kingMatrix;
	Matrix queenMatrix;
	Matrix startMatrix;
	Matrix endMatrix;

	TextureType diffuseTex;
	TextureType specTex;

	Matrix viewMatrix;
	Matrix projectionMatrix;

	// mouse variables
	Vector2 mousePos;
	Vector2 mouseDelta;
	bool buttonDown;

	// for camera controls
	Vector3 cameraPos;
	Vector2 cameraRotationSpeed;
	Vector2 cameraRotation;
	float cameraRadius;
	float cameraRadiusSpeed;

	void UpdateCamera(float deltaTime);

	// call when the mouse is released
	void OnMouseDown();
	void OnMouseMove();

};

#endif
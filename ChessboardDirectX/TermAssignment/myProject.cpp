#include "MyProject.h"
#include <Windowsx.h> // for GET__LPARAM macros
#include <d3d11_1.h>
#include <SimpleMath.h>
#include <DirectXColors.h>
#include <sstream>
#include <CommonStates.h>

/*
* Name: Brandon Keller
* Date: December 16, 2023
* 
* Due: December 16, 2023
* Course: GAME1201
* 
* Term Assignment 2 - Check Mate
*/


using namespace DirectX;
using namespace DirectX::SimpleMath;

// helper function
//
// returns a random float between 0 & 1
float RandFloat() { return float(rand())/float(RAND_MAX); } 

static const float CAMERA_SPEED = XM_PI * 0.2f;

//----------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
	MyProject application(hInstance);    // Create the class variable

	if( application.InitWindowsApp(L"CHESS 3D", nShowCmd) == false )    // Initialize the window, if all is well show and update it so it displays
	{
		return 0;                   // Error creating the window, terminate application
	}

	if( application.InitializeDirect3D() )
	{
		application.SetDepthStencil(true);      // Tell DirectX class to create and maintain a depth stencil buffer
		application.InitializeObjects();
		application.MessageLoop();				// Window has been successfully created, start the application message loop
	}

	return 0;
}

//----------------------------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------------------------
MyProject::MyProject(HINSTANCE hInstance)
	: DirectXClass(hInstance)
{
	mousePos = Vector2(clientWidth * 0.5f, clientHeight * 0.5f);
	buttonDown = false;
	
	ClearColor = Color(DirectX::Colors::Black.v);

	cameraPos = Vector3(0, 0, 6);

	cameraRadius = 32;
	cameraRadiusSpeed = 0;

	cameraRotation = Vector2(0, XM_PI / 10.0f );
	cameraRotationSpeed = Vector2::Zero;

	runTime = 0;
}

//----------------------------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------------------------
MyProject::~MyProject()
{
}

//----------------------------------------------------------------------------------------------
// Initialize any fonts we need to use here
//----------------------------------------------------------------------------------------------
void MyProject::InitializeObjects()
{
	skyBox.Initialize(D3DDevice, DeviceContext, L"..\\Textures\\envMap.dds", 64 );

	// load the shader
	shader.LoadShader(D3DDevice);

	// load chess board
	chessboard.Initialize(D3DDevice, &shader, Matrix::CreateScale(1, 0.5, 1), Colors::Beige.v, Colors::Brown.v); // beige and brown classic chessboard look

	// load player 1 chess pieces
	pawn.Initialize(D3DDevice, &shader, 2.25);
	bishop.Initialize(D3DDevice, &shader, 2.25);
	rook.Initialize(D3DDevice, &shader, 2.25);
	king.Initialize(D3DDevice, &shader, 2.25);
	queen.Initialize(D3DDevice, &shader, 2.25);
	knight.Initialize(D3DDevice, &shader, 2.25);
	
	// load player 2 chess pieces
	pawn2.Initialize(D3DDevice, &shader, 2.25);
	bishop2.Initialize(D3DDevice, &shader, 2.25);
	rook2.Initialize(D3DDevice, &shader, 2.25);
	king2.Initialize(D3DDevice, &shader, 2.25);
	queen2.Initialize(D3DDevice, &shader, 2.25);
	knight2.Initialize(D3DDevice, &shader, 2.25);

	// load the textures
	diffuseTex.Load(D3DDevice, DeviceContext, L"..\\Textures\\marble8.jpg");
	specTex.Load(D3DDevice, DeviceContext, L"..\\Textures\\marbleSpec.jpg");

	// player 1 chess pieces
	playerOneColour = Colors::White.v;
	pawn.SetTextures(diffuseTex.GetResourceView(), specTex.GetResourceView());
	bishop.SetTextures(diffuseTex.GetResourceView(), specTex.GetResourceView());
	rook.SetTextures(diffuseTex.GetResourceView(), specTex.GetResourceView());
	king.SetTextures(diffuseTex.GetResourceView(), specTex.GetResourceView());
	queen.SetTextures(diffuseTex.GetResourceView(), specTex.GetResourceView());
	knight.SetTextures(diffuseTex.GetResourceView(), specTex.GetResourceView());
	
	// player 2 chess pieces
	playerTwoColour = Colors::Black.v;
	pawn2.SetTextures(diffuseTex.GetResourceView(), specTex.GetResourceView());
	bishop2.SetTextures(diffuseTex.GetResourceView(), specTex.GetResourceView());
	rook2.SetTextures(diffuseTex.GetResourceView(), specTex.GetResourceView());
	king2.SetTextures(diffuseTex.GetResourceView(), specTex.GetResourceView());
	queen2.SetTextures(diffuseTex.GetResourceView(), specTex.GetResourceView());
	knight2.SetTextures(diffuseTex.GetResourceView(), specTex.GetResourceView());
	
	// chessboard
	chessboard.SetTextures(diffuseTex.GetResourceView(), specTex.GetResourceView());

	// set the matrices
	startMatrix =Matrix::CreateRotationZ(45.0f *XM_PI / 180.0f) * Matrix::CreateTranslation(-12.0f, 0, 0);
	endMatrix = Matrix::CreateRotationZ(-45.0f * XM_PI / 180.0f) * Matrix::CreateTranslation(12.0f, 0, 0);
	pawnMatrix = startMatrix;
	rookMatrix = startMatrix;
	knightMatrix = startMatrix;
	bishopMatrix = startMatrix;
	kingMatrix = startMatrix;
	queenMatrix = startMatrix;

}

//----------------------------------------------------------------------------------------------
// Window message handler
//----------------------------------------------------------------------------------------------
LRESULT MyProject::ProcessWindowMessages(UINT msg, WPARAM wParam, LPARAM lParam)
{
	// for mouse positions
	Vector2 pos((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));

	switch (msg )
	{
	case WM_MOUSEMOVE:
		mouseDelta = pos - mousePos;
		mousePos = pos;
		OnMouseMove();
		return 0;
	case WM_LBUTTONUP:
		buttonDown = false;
		mouseDelta = pos - mousePos;
		mousePos = pos;
		break;
	case WM_LBUTTONDOWN:
		buttonDown = true;
		mouseDelta = pos - mousePos;
		mousePos = pos;
		OnMouseDown();
		break;
	case WM_KEYUP:
		if (wParam >= '0' && wParam <= '4')
		{
			PresentInterval = wParam - '0';
		}
		else if (wParam == VK_UP)	{	cameraRotationSpeed.y = 0;	}
		else if (wParam == VK_DOWN) {	cameraRotationSpeed.y = 0; }
		else if (wParam == VK_LEFT) {	cameraRotationSpeed.x = 0; }
		else if (wParam == VK_RIGHT){	cameraRotationSpeed.x = 0; }
		else if (wParam == VK_ADD)  {	cameraRadiusSpeed = 0; }
		else if (wParam == VK_SUBTRACT)  { cameraRadiusSpeed = 0; }
		else if (wParam == VK_SPACE)
		{
			cameraRotation = Vector2::Zero;
			cameraRadius   = 6;
		}

		break;
	case WM_KEYDOWN:
		if (wParam == VK_UP)	{ cameraRotationSpeed.y = CAMERA_SPEED; }
		else if (wParam == VK_DOWN) { cameraRotationSpeed.y = -CAMERA_SPEED; }
		else if (wParam == VK_LEFT) { cameraRotationSpeed.x = -CAMERA_SPEED; }
		else if (wParam == VK_RIGHT){ cameraRotationSpeed.x = CAMERA_SPEED; }
		else if (wParam == VK_ADD)  { cameraRadiusSpeed = -1.0f; }
		else if (wParam == VK_SUBTRACT)  { cameraRadiusSpeed = 1.0f; }
		break;

	}

	// let the base class handle remaining messages
	return DirectXClass::ProcessWindowMessages(msg, wParam, lParam);
}

//----------------------------------------------------------------------------------------------
// Called by the render loop to render a single frame
//----------------------------------------------------------------------------------------------
void MyProject::Render(void)
{
	// calculate camera matrices
	ComputeViewProjection();

	// draw the skybox FIRST
	skyBox.Draw(DeviceContext, viewMatrix, projectionMatrix);

	ID3D11ShaderResourceView* pTex = diffuseTex.GetResourceView();
	DeviceContext->PSSetShaderResources(0, 1, &pTex);

	// chessboard
	shader.SetAmbientLight(Colors::White.v);
	chessboard.Draw(DeviceContext, Matrix::Identity, viewMatrix, projectionMatrix);

	// player 2 chess pieces
	shader.SetAmbientLight(playerTwoColour);
	for (int i = 0; i < 8; i++)
	{
		pawn2.Draw(DeviceContext, chessboard.GetBoardPosition(i, 1, pawn.GetBaseOffset()), viewMatrix, projectionMatrix);
	}
	bishop2.Draw(DeviceContext, chessboard.GetBoardPosition(2, 0, bishop.GetBaseOffset()), viewMatrix, projectionMatrix);
	bishop2.Draw(DeviceContext, chessboard.GetBoardPosition(5, 0, bishop.GetBaseOffset()), viewMatrix, projectionMatrix);

	rook2.Draw(DeviceContext, chessboard.GetBoardPosition(0, 0, rook.GetBaseOffset()), viewMatrix, projectionMatrix); // left side
	rook2.Draw(DeviceContext, chessboard.GetBoardPosition(7, 0, rook.GetBaseOffset()), viewMatrix, projectionMatrix); // right side

	knight2.Draw(DeviceContext, Matrix::CreateRotationY(XM_PI) * chessboard.GetBoardPosition(1, 0, knight.GetBaseOffset()), viewMatrix, projectionMatrix); // left side
	knight2.Draw(DeviceContext, Matrix::CreateRotationY(XM_PI) * chessboard.GetBoardPosition(6, 0, knight.GetBaseOffset()), viewMatrix, projectionMatrix); // right side

	king2.Draw(DeviceContext, chessboard.GetBoardPosition(4, 0, king.GetBaseOffset()), viewMatrix, projectionMatrix);
	queen2.Draw(DeviceContext, chessboard.GetBoardPosition(3, 0, queen.GetBaseOffset()), viewMatrix, projectionMatrix);

	// player 1 chess pieces
	shader.SetAmbientLight(playerOneColour);
	for (int i = 0; i < 8; i++)
	{
		pawn.Draw(DeviceContext, chessboard.GetBoardPosition(i, 6, pawn.GetBaseOffset()), viewMatrix, projectionMatrix);
	}

	bishop.Draw(DeviceContext, chessboard.GetBoardPosition(2, 7, bishop.GetBaseOffset()), viewMatrix, projectionMatrix); // left side
	bishop.Draw(DeviceContext, chessboard.GetBoardPosition(5, 7, bishop.GetBaseOffset()), viewMatrix, projectionMatrix); // right side

	rook.Draw(DeviceContext, chessboard.GetBoardPosition(0, 7, rook.GetBaseOffset()), viewMatrix, projectionMatrix); // left side
	rook.Draw(DeviceContext, chessboard.GetBoardPosition(7, 7, rook.GetBaseOffset()), viewMatrix, projectionMatrix); // right side

	knight.Draw(DeviceContext, chessboard.GetBoardPosition(1, 7, knight.GetBaseOffset()), viewMatrix, projectionMatrix); // left side
	knight.Draw(DeviceContext, chessboard.GetBoardPosition(6, 7, knight.GetBaseOffset()), viewMatrix, projectionMatrix); // right side

	king.Draw(DeviceContext, chessboard.GetBoardPosition(4, 7, king.GetBaseOffset()), viewMatrix, projectionMatrix);
	queen.Draw(DeviceContext, chessboard.GetBoardPosition(3, 7, queen.GetBaseOffset()), viewMatrix, projectionMatrix);

	// chess title font
	font.PrintMessage(clientWidth/2, 60, L"CHESS", Colors::LightGray);

	// render the base class
	DirectXClass::Render();
}

//----------------------------------------------------------------------------------------------
// Given a running value, returns a value between 0 and 1 which 'ping pongs' back and forth
//----------------------------------------------------------------------------------------------
float PingPong(float value)
{	
	value = fabs(value);
	int wholeNumber = (int)value;	// integer component
	float decimal = value - (float)wholeNumber; // decimal parts

	// if it's odd, flip it 
	if (wholeNumber % 2 == 1)
		decimal = 1.0f - decimal;

	return decimal;
}

//----------------------------------------------------------------------------------------------
// Linear Interpolate Matrices, decomposing if possible to give nondistorting results
//----------------------------------------------------------------------------------------------
Matrix LerpMatrices(Matrix& start, Matrix& end, float t)
{
	Vector3 scaleA, scaleB;
	Quaternion rotA, rotB;
	Vector3 transA, transB;

	Matrix result;
	if (start.Decompose(scaleA, rotA, transA) && end.Decompose(scaleB, rotB, transB))
	{
		Vector3 scale = Vector3::Lerp(scaleA, scaleB, t);
		Quaternion rot = Quaternion::Lerp(rotA, rotB, t);
		Vector3 trans = Vector3::Lerp(transA, transB, t);
		result = Matrix::CreateScale(scale)*Matrix::CreateFromQuaternion(rot)*Matrix::CreateTranslation(trans);
	}
	else
	{
		result = Matrix::Lerp(start, end, t);
	}
	return result;
}

//----------------------------------------------------------------------------------------------
// Called every frame to update objects.
//	deltaTime: how much time in seconds has elapsed since the last frame
//----------------------------------------------------------------------------------------------
void MyProject::Update(float deltaTime)
{
	Vector3 dir2 = { -1,1,-1 };
	Vector3 dir = { 0, -1,0 };

	// how long one movement takes
	const float MOVEMENT_TIME_IN_SECONDS = 3.0f; 
	runTime += deltaTime;

	// get a 0 - 1 value
	float blendValue = PingPong(runTime / MOVEMENT_TIME_IN_SECONDS);

	// Linear Interpolate between the matrices
	pawnMatrix = LerpMatrices(startMatrix, endMatrix, blendValue);

	// update the camera movement
	UpdateCamera(deltaTime);

	// update the sample object
	pawn.Update(deltaTime);


	shader.SetAmbientLight(Colors::LightBlue.v);
	shader.SetDirectionalLight(Colors::Green.v, dir);
	// set the spec color and power
	shader.SetSpecularLight(Colors::Yellow.v, 64);
}


//----------------------------------------------------------------------------------------------
// Called when the mouse is released
//----------------------------------------------------------------------------------------------
void MyProject::OnMouseDown()
{

	// this is called when the left mouse button is clicked
	// mouse position is stored in mousePos variable
}

//----------------------------------------------------------------------------------------------
// Called when the mouse is moved
//----------------------------------------------------------------------------------------------
void MyProject::OnMouseMove()
{
}

//----------------------------------------------------------------------------------------------
// Computes the view and camera matrix
//----------------------------------------------------------------------------------------------
void MyProject::ComputeViewProjection()
{
	viewMatrix = Matrix::CreateLookAt(cameraPos, Vector3::Zero, Vector3::UnitY);
	projectionMatrix = Matrix::CreatePerspectiveFieldOfView(60.0f * XM_PI / 180.0f, (float)clientWidth / (float)clientHeight, 1, 128);
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void MyProject::UpdateCamera(float deltaTime)
{
	const float VERT_LIMIT = XM_PI * 0.35f;

	// update the radius
	cameraRadius += cameraRadiusSpeed * deltaTime;
	if (cameraRadius < 1) cameraRadius = 1;


	// update the rotation amounts
	cameraRotation += cameraRotationSpeed * deltaTime;

	// clamp the vertical rotation
	if (cameraRotation.y < -VERT_LIMIT) cameraRotation.y = -VERT_LIMIT;
	else if (cameraRotation.y > VERT_LIMIT) cameraRotation.y = VERT_LIMIT;

	// calculate the height
	cameraPos.y = cameraRadius * sinf(cameraRotation.y);
	float r = cameraRadius * cosf(cameraRotation.y);

	// calculate the orbit
	cameraPos.x = sinf(cameraRotation.x) * r;
	cameraPos.z = cosf(cameraRotation.x) * r;
}
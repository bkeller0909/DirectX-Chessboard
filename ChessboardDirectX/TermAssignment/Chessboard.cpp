//
// ChessboardModel
//
//  BGTD 9201
//

#include "Chessboard.h"

struct MaterialBuffer
{
	Color ambient;
	Color diffuse;
	Color spec; // alpha in w component

};

// called to initialize the object
void Chessboard::Initialize(ID3D11Device* pDevice, LitColourShader* pLitShader, Matrix inWorldMatrix, Color colour1, Color colour2)
{
	gridColour1 = colour1;
	gridColour2 = colour2;

	pShader = pLitShader;

	gridScale = 4;

	float xOffset = X_LENGTH / 2.0f - 0.5f;
	float yOffset = Y_LENGTH / 2.0f - 0.5f;

	for (int x = 0; x < X_LENGTH; x++) {
		for (int y = 0; y < Y_LENGTH; y++) {
			chessGrid[x][y].InitializeGeometry(pDevice, Cube);
			chessGrid[x][y].InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());
			chessGridMatrix[x][y] = Matrix::CreateScale(gridScale) * Matrix::CreateTranslation((x - xOffset) * gridScale, -1, (y - yOffset) * gridScale);
		}
	}

	worldPositionMatrix = inWorldMatrix;

	base.InitializeGeometry(pDevice, Cylinder);
	base.InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());
	baseMatrix = Matrix::CreateScale(2.5, 0.5, 2.5) * Matrix::CreateTranslation(0, -1.5, 0);
	pBaseBuffer = MakeMaterialBuffer(pDevice, Colors::DarkGreen.v, Colors::DarkGreen.v, Colors::Black.v, 2);

	middle.InitializeGeometry(pDevice, Cylinder);
	middle.InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());
	middleMatrix = Matrix::CreateScale(1, 2.5, 1);
	pMiddleBuffer = MakeMaterialBuffer(pDevice, Colors::DarkGoldenrod.v, Colors::Goldenrod.v, Colors::DarkGoldenrod.v, 8);

	top.InitializeGeometry(pDevice, Sphere);
	top.InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());
	topMatrix = Matrix::CreateScale(2.0f, 2.0f, 2.0f) * Matrix::CreateTranslation(0, 1.75, 0);
	pTopBuffer = MakeMaterialBuffer(pDevice, Colors::Black.v, Colors::DarkGray.v, Colors::Silver.v, 128);

}

// called to draw the object
// The parent matrix allows the user to pass in a matrix to transform the entire piece
void Chessboard::Draw(ID3D11DeviceContext* pDeviceContext, const Matrix& parentMatrix, const Matrix& viewMatrix, const Matrix& projMatrix)
{
	// set all 3 to the diffuse
	pDeviceContext->PSSetShaderResources(0, 1, &pDiffuse);
	pDeviceContext->PSSetShaderResources(1, 1, &pDiffuse);
	pDeviceContext->PSSetShaderResources(2, 1, &pDiffuse);

	pDeviceContext->PSSetConstantBuffers(2, 1, &pBaseBuffer);
	
	// chess
	for (int x = 0; x < X_LENGTH; x++) {
		for (int y = 0; y < Y_LENGTH; y++) {
			if ((x + y) % 2 == 0) {
				pShader->SetAmbientLight(gridColour1);
			}
			else {
				pShader->SetAmbientLight(gridColour2);
			}

			pShader->SetShaders(pDeviceContext, chessGridMatrix[x][y] * worldPositionMatrix, viewMatrix, projMatrix);
			chessGrid[x][y].Draw(pDeviceContext);
		}
	}
}

// update the object
void Chessboard::Update(float deltaTime)
{
}

Matrix Chessboard::GetBoardPosition(int x, int y, float pieceBaseOffset)
{
	float xOffset = X_LENGTH / 2.0f - 0.5f;
	float yOffset = Y_LENGTH / 2.0f - 0.5f;

	return Matrix::CreateTranslation((x - xOffset) * gridScale, pieceBaseOffset, (y - yOffset) * gridScale);
}

// Helper to make a buffer from the given materials
//
ID3D11Buffer* Chessboard::MakeMaterialBuffer(ID3D11Device* pDevice, Color ambient, Color diffuse, Color spec, float specPower)
{
	MaterialBuffer mat;
	mat.ambient = ambient;
	mat.diffuse = diffuse;
	mat.spec = spec;
	mat.spec.w = specPower;

	// Create the constant buffer
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(MaterialBuffer);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &mat;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	ID3D11Buffer* pConstantBuffer;
	pDevice->CreateBuffer(&bufferDesc, &data, &pConstantBuffer);
	return pConstantBuffer;
}

// constructor
Chessboard::Chessboard()
{
	pShader = nullptr;
	pBaseBuffer = nullptr;
	pMiddleBuffer = nullptr;
	pTopBuffer = nullptr;
	pDiffuse = nullptr;
	pSpec = nullptr;
}

// destructo
Chessboard::~Chessboard()
{
	if (pBaseBuffer) pBaseBuffer->Release();
	if (pMiddleBuffer) pMiddleBuffer->Release();
	if (pTopBuffer) pTopBuffer->Release();
}
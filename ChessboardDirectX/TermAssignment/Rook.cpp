//
// RookModel
//
//  BGTD 9201
//

#include "Rook.h"

struct MaterialBuffer
{
	Color ambient;
	Color diffuse;
	Color spec; // alpha in w component

};

// called to initialize the object
void Rook::Initialize(ID3D11Device* pDevice, LitColourShader* pLitShader, float baseOffset)
{
	pShader = pLitShader;

	// this is all taken from my assignment 5
	for (int i = 0; i < NUM_BOTTOM_PARTS; i++)
	{
		base[i].InitializeGeometry(pDevice, Cylinder);
		base[i].InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());
	}
	baseMatrix[0] = Matrix::CreateScale(2.15, 0.25, 1.75) * Matrix::CreateTranslation(0, -1, 0);;
	baseMatrix[1] = Matrix::CreateScale(1.85, 0.25, 1.65) * Matrix::CreateTranslation(0, -1.25, 0);
	baseMatrix[2] = Matrix::CreateScale(2.0, 0.25, 1.75) * Matrix::CreateTranslation(0, -1.5, 0);
	pBaseBuffer = MakeMaterialBuffer(pDevice, Colors::DarkGreen.v, Colors::DarkGreen.v, Colors::Black.v, 2);

	for (int i = 0; i < NUM_MIDDLE_PARTS; i++)
	{
		middle[i].InitializeGeometry(pDevice, Cylinder);
		middle[i].InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());
	}
	middleMatrix[0] = Matrix::CreateScale(1.5, 4.0, 1.5) * Matrix::CreateTranslation(0, 1, 0);
	pMiddleBuffer = MakeMaterialBuffer(pDevice, Colors::DarkGoldenrod.v, Colors::Goldenrod.v, Colors::DarkGoldenrod.v, 8);

	for (int i = 0; i < NUM_TOP_PARTS; i++)
	{
		top[i].InitializeGeometry(pDevice, Cylinder);
		top[i].InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());
	}
	topMatrix[0] = Matrix::CreateScale(1.75, 0.25, 1.75) * Matrix::CreateTranslation(0, 2.60, 0);
	topMatrix[1] = Matrix::CreateScale(1.65, 0.25, 1.65) * Matrix::CreateTranslation(0, 2.75, 0);
	topMatrix[2] = Matrix::CreateScale(1.75, 0.25, 1.75) * Matrix::CreateTranslation(0, 3, 0);

	// crown part
	topMatrix[3] = Matrix::CreateScale(0.6, 0.25, 0.25) * Matrix::CreateRotationY(90 * XM_PI / 180) * Matrix::CreateTranslation(0.75, 3.25, 0.0); // right
	topMatrix[4] = Matrix::CreateScale(0.6, 0.25, 0.25) * Matrix::CreateTranslation(0, 3.25, -0.75); // top
	topMatrix[5] = Matrix::CreateScale(0.6, 0.25, 0.25) * Matrix::CreateTranslation(0, 3.25, 0.75); // bottom
	topMatrix[6] = Matrix::CreateScale(0.6, 0.25, 0.25) * Matrix::CreateRotationY(90 * XM_PI / 180) * Matrix::CreateTranslation(-0.75, 3.25, 0); // left
	pTopBuffer = MakeMaterialBuffer(pDevice, Colors::Black.v, Colors::DarkGray.v, Colors::Silver.v, 128);

	SetBaseOffset(baseOffset);
}

// called to draw the object
// The parent matrix allows the user to pass in a matrix to transform the entire piece
void Rook::Draw(ID3D11DeviceContext* pDeviceContext, const Matrix& parentMatrix, const Matrix& viewMatrix, const Matrix& projMatrix)
{
	// set all 3 to the diffuse
	pDeviceContext->PSSetShaderResources(0, 1, &pDiffuse);
	pDeviceContext->PSSetShaderResources(1, 1, &pDiffuse);
	pDeviceContext->PSSetShaderResources(2, 1, &pDiffuse);

	// bottom parts of chess piece
	pDeviceContext->PSSetConstantBuffers(2, 1, &pBaseBuffer);
	for (int i = 0; i < NUM_BOTTOM_PARTS; i++)
	{
		pShader->SetShaders(pDeviceContext, baseMatrix[i] * parentMatrix, viewMatrix, projMatrix);
		base[i].Draw(pDeviceContext);
	}

	// middle parts of chess piece
	pDeviceContext->PSSetConstantBuffers(2, 1, &pMiddleBuffer);
	for (int i = 0; i < NUM_MIDDLE_PARTS; i++)
	{
		pShader->SetShaders(pDeviceContext, middleMatrix[i] * parentMatrix, viewMatrix, projMatrix);
		middle[i].Draw(pDeviceContext);
	}

	// change up the spec
	pDeviceContext->PSSetShaderResources(2, 1, &pSpec);

	// top parts of chess piece
	pDeviceContext->PSSetConstantBuffers(2, 1, &pTopBuffer);
	for (int i = 0; i < NUM_TOP_PARTS; i++)
	{
		pShader->SetShaders(pDeviceContext, topMatrix[i] * parentMatrix, viewMatrix, projMatrix);
		top[i].Draw(pDeviceContext);
	}
}

// update the object
void Rook::Update(float deltaTime)
{
}

// Helper to make a buffer from the given materials
//
ID3D11Buffer* Rook::MakeMaterialBuffer(ID3D11Device* pDevice, Color ambient, Color diffuse, Color spec, float specPower)
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
Rook::Rook()
{
	pShader = nullptr;
	pBaseBuffer = nullptr;
	pMiddleBuffer = nullptr;
	pTopBuffer = nullptr;
	pDiffuse = nullptr;
	pSpec = nullptr;
}

// destructo
Rook::~Rook()
{
	if (pBaseBuffer) pBaseBuffer->Release();
	if (pMiddleBuffer) pMiddleBuffer->Release();
	if (pTopBuffer) pTopBuffer->Release();
}
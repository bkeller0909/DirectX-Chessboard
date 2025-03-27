//
// KnightModel
//
//  BGTD 9201
//

#include "Knight.h"

struct MaterialBuffer
{
	Color ambient;
	Color diffuse;
	Color spec; // alpha in w component

};

// called to initialize the object
void Knight::Initialize(ID3D11Device* pDevice, LitColourShader* pLitShader, float baseOffset)
{
	pShader = pLitShader;

	// base
	for (int i = 0; i < NUM_BOTTOM_PARTS; i++)
	{
		base[i].InitializeGeometry(pDevice, Cylinder);
		base[i].InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());
	}
	baseMatrix[0] = Matrix::CreateScale(2.5, 0.5, 2.5) * Matrix::CreateTranslation(0, -1.5, 0);
	pBaseBuffer = MakeMaterialBuffer(pDevice, Colors::DarkGreen.v, Colors::DarkGreen.v, Colors::Black.v, 2);

	// middle
	for (int i = 0; i < NUM_MIDDLE_PARTS; i++)
	{
		middle[i].InitializeGeometry(pDevice, Cube);
		middle[i].InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());
	}
	middleMatrix[0] = Matrix::CreateScale(1.2, 2.5, 1.2) * Matrix::CreateRotationX(35) * Matrix::CreateTranslation(0, 0, 0.5);
	middleMatrix[1] = Matrix::CreateScale(1, 2, 1) * Matrix::CreateTranslation(0, 2, 1);
	middleMatrix[2] = Matrix::CreateScale(1, 1, 1.25) * Matrix::CreateTranslation(0, 2.5, 0);
	pMiddleBuffer = MakeMaterialBuffer(pDevice, Colors::DarkGoldenrod.v, Colors::Goldenrod.v, Colors::DarkGoldenrod.v, 8);

	// top part for ears of knight piece
	for (int i = 0; i < NUM_TOP_PARTS; i++)
	{
		top[i].InitializeGeometry(pDevice, Cube);
		top[i].InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());
	}
	topMatrix[0] = Matrix::CreateScale(0.25, 1, 0.25) * Matrix::CreateTranslation(0.5, 3.4, 1); // left ear
	topMatrix[1] = Matrix::CreateScale(0.25, 1, 0.25) * Matrix::CreateTranslation(-0.5, 3.4, 1); // right ear
	pTopBuffer = MakeMaterialBuffer(pDevice, Colors::Black.v, Colors::DarkGray.v, Colors::Silver.v, 128);

	SetBaseOffset(baseOffset);
}

// called to draw the object
// The parent matrix allows the user to pass in a matrix to transform the entire piece
void Knight::Draw(ID3D11DeviceContext* pDeviceContext, const Matrix& parentMatrix, const Matrix& viewMatrix, const Matrix& projMatrix)
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
void Knight::Update(float deltaTime)
{
}

// Helper to make a buffer from the given materials
//
ID3D11Buffer* Knight::MakeMaterialBuffer(ID3D11Device* pDevice, Color ambient, Color diffuse, Color spec, float specPower)
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
Knight::Knight()
{
	pShader = nullptr;
	pBaseBuffer = nullptr;
	pMiddleBuffer = nullptr;
	pTopBuffer = nullptr;
	pDiffuse = nullptr;
	pSpec = nullptr;
}

// destructo
Knight::~Knight()
{
	if (pBaseBuffer) pBaseBuffer->Release();
	if (pMiddleBuffer) pMiddleBuffer->Release();
	if (pTopBuffer) pTopBuffer->Release();
}
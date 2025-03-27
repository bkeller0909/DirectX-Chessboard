//
// QueenModel
//
//  BGTD 9201
//

#include "Queen.h"

struct MaterialBuffer
{
	Color ambient;
	Color diffuse;
	Color spec; // alpha in w component

};

// called to initialize the object
void Queen::Initialize(ID3D11Device* pDevice, LitColourShader* pLitShader, float baseOffset)
{
	pShader = pLitShader;

	// base of the queen
	for (int i = 0; i < NUM_BOTTOM_PARTS; i++)
	{
		base[i].InitializeGeometry(pDevice, Cylinder);
		base[i].InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());
	}
	baseMatrix[0] = Matrix::CreateScale(2.5, 0.5, 2.5) * Matrix::CreateTranslation(0, -1.5, 0);
	pBaseBuffer = MakeMaterialBuffer(pDevice, Colors::DarkGreen.v, Colors::DarkGreen.v, Colors::Black.v, 2);

	// middle parts
	for (int i = 0; i < NUM_MIDDLE_PARTS; i++)
	{
		middle[i].InitializeGeometry(pDevice, Cylinder);
		middle[i].InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());
	}
	middleMatrix[0] = Matrix::CreateScale(1, 3, 1);
	middleMatrix[1] = Matrix::CreateScale(2, 0.2, 2) * Matrix::CreateTranslation(0, 1.5, 0);
	middleMatrix[2] = Matrix::CreateScale(1, 0.3, 1) * Matrix::CreateTranslation(0, 1.75, 0);
	middleMatrix[3] = Matrix::CreateScale(1.8, 2, 1.8) * Matrix::CreateTranslation(0, 2.5, 0);
	pMiddleBuffer = MakeMaterialBuffer(pDevice, Colors::DarkGoldenrod.v, Colors::Goldenrod.v, Colors::DarkGoldenrod.v, 8);

	
	// crown for the top of the head
	const float radius = 0.9; // radius for the crown to fit on the piece's head
	const int crownParts = NUM_TOP_PARTS - 1; // uses the array for the crown parts except for the last one

	// loop for the spheres to become a perfect circle
	for (int i = 0; i < crownParts; i++)
	{
		float angle = i / static_cast<float>(crownParts) * 2 * XM_PI; // calculate angle in radians
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		top[i].InitializeGeometry(pDevice, Sphere);
		top[i].InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());

		topMatrix[i] = Matrix::CreateScale(0.4, 0.4, 0.4) * Matrix::CreateTranslation(x, 3.5, z);
	}
	// top of the head piece
	top[12].InitializeGeometry(pDevice, Sphere);
	top[12].InitializeInputLayout(pDevice, pShader->GetVertexShaderBinary(), pShader->GetVertexShaderBinarySize());
	topMatrix[12] = Matrix::CreateScale(0.75, 0.75, 0.75) * Matrix::CreateTranslation(0, 3.75, 0);

	pTopBuffer = MakeMaterialBuffer(pDevice, Colors::Black.v, Colors::DarkGray.v, Colors::Silver.v, 128);

	SetBaseOffset(baseOffset);
}

// called to draw the object
// The parent matrix allows the user to pass in a matrix to transform the entire piece
void Queen::Draw(ID3D11DeviceContext* pDeviceContext, const Matrix& parentMatrix, const Matrix& viewMatrix, const Matrix& projMatrix)
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
void Queen::Update(float deltaTime)
{
}

// Helper to make a buffer from the given materials
//
ID3D11Buffer* Queen::MakeMaterialBuffer(ID3D11Device* pDevice, Color ambient, Color diffuse, Color spec, float specPower)
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
Queen::Queen()
{
	pShader = nullptr;
	pBaseBuffer = nullptr;
	pMiddleBuffer = nullptr;
	pTopBuffer = nullptr;
	pDiffuse = nullptr;
	pSpec = nullptr;
}

// destructo
Queen::~Queen()
{
	if (pBaseBuffer) pBaseBuffer->Release();
	if (pMiddleBuffer) pMiddleBuffer->Release();
	if (pTopBuffer) pTopBuffer->Release();
}
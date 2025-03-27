//
// BTGD 9201 - Object that represents a skybox object
//

#include "SkyBox.h"
#include <D3Dcompiler.h>

// constant buffer structure
struct ConstantBuffer
{
	Matrix worldMatrix;
	Matrix mvpMatrix;
	Vector4	cameraPos;
};

// ---------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------
SkyBox::SkyBox()
{
	pPixelShader = nullptr;
	pVertexShader  = nullptr;
	pConstants = nullptr;
	pSamplerState = nullptr;
	pVertexShaderBlob = nullptr;
	pPixelShaderBlob = nullptr;
	scale = 1;
}

// ---------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------
SkyBox::~SkyBox()
{
	if (pPixelShader) pPixelShader->Release();
	if (pVertexShader) pVertexShader->Release();
	if (pConstants) pConstants->Release();
	if (pSamplerState) pSamplerState->Release();
	if (pPixelShaderBlob) pPixelShaderBlob->Release();
	if (pVertexShaderBlob) pVertexShaderBlob->Release();


}

// ---------------------------------------------------------------------
// initialize the skybox with a texturename and size
// ---------------------------------------------------------------------
void SkyBox::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const wchar_t* textureName, int size)
{
	scale = float(size);

	// load the texture
	if (!texture.Load(pDevice, pDeviceContext, textureName))
	{
		OutputDebugString(L"Couldn't load skybox texture");
		assert(0);
		return;
	}

	// load the vertex shaders
	HRESULT hr = D3DReadFileToBlob(L"SkyBoxVS.cso", &pVertexShaderBlob);
	if (FAILED(hr))
	{
		OutputDebugString(L"Couldn't load vertex shader");
		assert(0);
		return;
	}

	// initialize the geometry to use a cube
	skyGeo.InitializeGeometry(pDevice, Cube);
	skyGeo.InitializeInputLayout(pDevice, pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize());

	// create the vertex shader
	pDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), NULL, &pVertexShader);

	// load the pixel shader
	hr = D3DReadFileToBlob(L"SkyBoxPS.cso", &pPixelShaderBlob);
	if (FAILED(hr))
	{
		OutputDebugString(L"Couldn't load pixel shader");
		assert(0);
		return;
	}

	// create the pixel shader
	pDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), NULL, &pPixelShader);


	// ShaderConstants create some initial data
	ConstantBuffer initData;

	// Create the constant buffer
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(ConstantBuffer);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &initData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	hr = pDevice->CreateBuffer(&bufferDesc, &data, &pConstants);
	if (FAILED(hr))
	{
		OutputDebugString(L"Couldn't create lights buffer");
		assert(0);
		return;
	}

	D3D11_SAMPLER_DESC sampler;
	sampler = CD3D11_SAMPLER_DESC(D3D11_DEFAULT);
	sampler.Filter = D3D11_FILTER_ANISOTROPIC;
	sampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler.MaxAnisotropy = 8;
	
	pDevice->CreateSamplerState(&sampler, &pSamplerState);


	states = std::unique_ptr<CommonStates>(new CommonStates(pDevice));
}

// ---------------------------------------------------------------------
// draw the skybox
// ---------------------------------------------------------------------
void SkyBox::Draw(ID3D11DeviceContext* pDeviceContext, const Matrix& viewMatrix, const Matrix& projMatrix)
{

	// tell direct x to update the constants inside the shader
	D3D11_MAPPED_SUBRESOURCE constantResource;

	// gets a pointer to write constants to
	pDeviceContext->Map(pConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantResource);
	ConstantBuffer* pValues = (ConstantBuffer*)constantResource.pData;

	// when setting the matrices we need to transpose them because the expected order is different in shaders than on CPU
	pValues->worldMatrix = Matrix::CreateScale(scale);
	pValues->mvpMatrix = (pValues->worldMatrix * viewMatrix * projMatrix).Transpose();
	Vector3 cam = Vector3::Transform(Vector3::Zero, viewMatrix.Invert());
	pValues->cameraPos = Vector4(cam.x, cam.y, cam.z, 1);

	// commit the changes
	pDeviceContext->Unmap(pConstants, 0);

	// set our textures, samplers and constant
	pDeviceContext->VSSetConstantBuffers(0, 1, &pConstants);
	pDeviceContext->PSSetSamplers(0, 1, &pSamplerState);
	
	ID3D11ShaderResourceView* pTex = texture.GetResourceView();
	pDeviceContext->PSSetShaderResources(0, 1, &pTex);

	// set the shaders
	pDeviceContext->VSSetShader(pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(pPixelShader, NULL, 0);

	// set our states - reverse the culling, no depth writes
	pDeviceContext->RSSetState(states->CullClockwise());
	pDeviceContext->OMSetDepthStencilState(states->DepthRead(), 0);

	skyGeo.Draw(pDeviceContext);

	// restore states
	pDeviceContext->RSSetState(states->CullCounterClockwise());
	pDeviceContext->OMSetDepthStencilState(states->DepthDefault(), 0);
}

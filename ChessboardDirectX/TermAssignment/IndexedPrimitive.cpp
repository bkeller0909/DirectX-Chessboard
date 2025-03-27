//
// BGTD 9201
//	Class that represents a simple IndexedPrimitive
//

#include "IndexedPrimitive.h"
#include <DirectXColors.h>
#include <VertexTypes.h>
#include <vector>
#include "Models.h"

static bool faceNormals = false;




// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
IndexedPrimitive::IndexedPrimitive()
{
	pVertexBuffer = nullptr;
	pInputLayout = nullptr;
	pIndexBuffer = nullptr;

	numVerts = 0;
	numIndices = 0;
}

// ------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------
IndexedPrimitive::~IndexedPrimitive()
{
	// Make sure we clean up what ever we allocated!
	if (pVertexBuffer != nullptr)
	{
		pVertexBuffer->Release();
		pVertexBuffer = nullptr;
	}
	if (pIndexBuffer != nullptr)
	{
		pIndexBuffer->Release();
		pIndexBuffer = nullptr;
	}
	if (pInputLayout != nullptr)
	{
		pInputLayout->Release();
		pInputLayout = nullptr;
	}
}

// ------------------------------------------------------------------------------------
// Initialize the vertex buffer
// ------------------------------------------------------------------------------------
void IndexedPrimitive::InitializeGeometry(ID3D11Device* pDevice, ModelType type)
{
	// build the icosahedron
	VertexCollection vertices;
	IndexCollection indices;
		
	// create the model
	switch (type)
	{
		case Cube: 
			Models::CreateCube(vertices, indices, 1.0f);
			break;
		case Torus:
			Models::CreateTorus(vertices, indices, 1.0f, 0.5f, 24);
			break;
		case Cone:
			Models::CreateCone(vertices, indices, 1, 1, 24);
			break;
		case Cylinder:
			Models::CreateCylinder(vertices, indices, 1, 1, 24);
			break;
		case Sphere:
			Models::CreateSphere(vertices, indices, 1, 24);
			break;
	}

	//
	numVerts = vertices.size();
	numIndices = indices.size();


	// describe the vertex buffer we are trying to create
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = numVerts * sizeof(VertexPositionNormalTexture);
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// setup the subresource data - tells D3D what data to use to initialize
	// the vertexbuffer with
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices.data();

	// create the vertex buffer
	HRESULT hr = pDevice->CreateBuffer(&desc, &data, &pVertexBuffer);
	if (FAILED(hr))
	{
		OutputDebugString(L"FAILED TO CREATE VERTEX BUFFER");
		assert(false);
	}


	// set up  the index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = numIndices * sizeof(uint16_t);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices.data();

	hr = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &pIndexBuffer);
	if (FAILED(hr))
	{
		OutputDebugString(L"FAILED TO CREATE INDEX BUFFER");
		assert(false);
	}
}

// ------------------------------------------------------------------------------------
// Initialzes the shaders
// ------------------------------------------------------------------------------------
void IndexedPrimitive::InitializeInputLayout(ID3D11Device* pDevice, const void* pBinary, size_t binarySize)
{
	// create the input layout
	HRESULT hr = pDevice->CreateInputLayout(VertexPositionNormalTexture::InputElements,
		VertexPositionNormalTexture::InputElementCount,
		pBinary, binarySize,
		&pInputLayout);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create input layout");
		assert(0);
	}
}

// ------------------------------------------------------------------------------------
// Draw the IndexedPrimitive
// ------------------------------------------------------------------------------------
void IndexedPrimitive::Draw(ID3D11DeviceContext* pDeviceContext)
{
	// Set up our input layout
	pDeviceContext->IASetInputLayout(pInputLayout);

	//  tell D3D we are drawing a triangle list
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//  Tell the device which vertex buffer we are using
	UINT stride = sizeof(VertexPositionNormalTexture);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	// Set the index buffer
	pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	//	tell it to draw the primitive
	pDeviceContext->DrawIndexed(numIndices, 0, 0);
}



//
// BGTD 9201
//	Class that represents a simple primitive
//

#include "Primitive.h"
#include <DirectXColors.h>
#include <VertexTypes.h>
#include <GeometricPrimitive.h>
#include <vector>

// Temporary collection types used when generating the geometry.
typedef std::vector<VertexPositionNormalTexture> VertexCollection;
typedef std::vector<uint16_t> IndexCollection;

static void CreateIcosahedron(VertexCollection& vertices, , float size, bool rhcoords)
{
	VertexCollection vertices;
	IndexCollection indices;

	static const float  t = 1.618033988749894848205f; // (1 + sqrt(5)) / 2
	static const float t2 = 1.519544995837552493271f; // sqrt( 1 + sqr( (1 + sqrt(5)) / 2 ) )

	static const XMVECTORF32 verts[12] =
	{
		{ t / t2, 1.f / t2, 0 },
		{ -t / t2, 1.f / t2, 0 },
		{ t / t2, -1.f / t2, 0 },
		{ -t / t2, -1.f / t2, 0 },
		{ 1.f / t2, 0, t / t2 },
		{ 1.f / t2, 0, -t / t2 },
		{ -1.f / t2, 0, t / t2 },
		{ -1.f / t2, 0, -t / t2 },
		{ 0, t / t2, 1.f / t2 },
		{ 0, -t / t2, 1.f / t2 },
		{ 0, t / t2, -1.f / t2 },
		{ 0, -t / t2, -1.f / t2 }
	};

	static const uint32_t faces[20 * 3] =
	{
		0, 8, 4,
		0, 5, 10,
		2, 4, 9,
		2, 11, 5,
		1, 6, 8,
		1, 10, 7,
		3, 9, 6,
		3, 7, 11,
		0, 10, 8,
		1, 8, 10,
		2, 9, 11,
		3, 11, 9,
		4, 2, 0,
		5, 0, 2,
		6, 1, 3,
		7, 3, 1,
		8, 6, 4,
		9, 4, 6,
		10, 5, 7,
		11, 7, 5
	};

	for (size_t j = 0; j < _countof(faces); j += 3)
	{
		uint32_t v0 = faces[j];
		uint32_t v1 = faces[j + 1];
		uint32_t v2 = faces[j + 2];

		XMVECTOR normal = XMVector3Cross(verts[v1].v - verts[v0].v,
			verts[v2].v - verts[v0].v);
		normal = XMVector3Normalize(normal);

		size_t base = vertices.size();
		indices.push_back(base);
		indices.push_back(base + 1);
		indices.push_back(base + 2);

		// Duplicate vertices to use face normals
		XMVECTOR position = XMVectorScale(verts[v0], size);
		vertices.push_back(VertexPositionNormalTexture(position, normal, g_XMZero /* 0, 0 */));

		position = XMVectorScale(verts[v1], size);
		vertices.push_back(VertexPositionNormalTexture(position, normal, g_XMIdentityR0 /* 1, 0 */));

		position = XMVectorScale(verts[v2], size);
		vertices.push_back(VertexPositionNormalTexture(position, normal, g_XMIdentityR1 /* 0, 1 */));
	}

	assert(vertices.size() == 20 * 3);
	assert(indices.size() == 20 * 3);
}

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
Primitive::Primitive()
{
	pVertexBuffer = nullptr;
	pInputLayout = nullptr;
}

// ------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------
Primitive::~Primitive()
{
	// Make sure we clean up what ever we allocated!
	if (pVertexBuffer != nullptr)
	{
		pVertexBuffer->Release();
		pVertexBuffer = nullptr;
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
void Primitive::InitializeGeometry(ID3D11Device* pDevice)
{	


	// Step 3
	// Create the buffer

	// describe the vertex buffer we are trying to create
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = NUM_VERTS * sizeof(VertexPositionNormalColor);
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// setup the subresource data - tells D3D what data to use to initialize
	// the vertexbuffer with
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices;

	// create the vertex buffer
	HRESULT hr = pDevice->CreateBuffer(&desc, &data, &pVertexBuffer);
	if (FAILED(hr))
	{
		OutputDebugString(L"FAILED TO CREATE VERTEX BUFFER");
		assert(false);
	}
}

// ------------------------------------------------------------------------------------
// Initialzes the shaders
// ------------------------------------------------------------------------------------
void Primitive::InitializeInputLayout(ID3D11Device* pDevice, const void* vsBinary, size_t vsBinarySize)
{
	// create the input layout object
	HRESULT hr = pDevice->CreateInputLayout(
		VertexPositionNormalColor::InputElements, VertexPositionNormalColor::InputElementCount,
		vsBinary, vsBinarySize,
		&pInputLayout
		);
	if (FAILED(hr))
	{
		OutputDebugString(L"Could not create input layout");
		assert(false);
	}
}

// ------------------------------------------------------------------------------------
// Draw the primitive
// ------------------------------------------------------------------------------------
void Primitive::Draw(ID3D11DeviceContext* pDeviceContext)
{
	std::unique_ptr<DirectX::GeometricPrimitive> prim = DirectX::GeometricPrimitive::CreateTeapot(pDeviceContext, 1, 8, true);



	// Set up our input layout
	pDeviceContext->IASetInputLayout(pInputLayout);

	//  tell D3D we are drawing a triangle list
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//  Tell the device which vertex buffer we are using
	UINT stride = sizeof(VertexPositionNormalColor);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	// Step 11
	//	tell it to draw the first three vertices
	pDeviceContext->Draw(NUM_VERTS, 0);
}



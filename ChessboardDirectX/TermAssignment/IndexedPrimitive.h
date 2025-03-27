//
// BGTD 9201
//	Class that represents a simple indexed primitive
//
#ifndef _INDEXED_PRIMITIVE_H
#define _INDEXED_PRIMITIVE_H

#include <d3d11_1.h>
#include <SimpleMath.h>
#include <Effects.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;


enum ModelType
{
	Cube,
	Torus,
	Cone,
	Cylinder,
	Sphere
};


class IndexedPrimitive
{
public:
	IndexedPrimitive();
	~IndexedPrimitive();

	// initialze the geometry
	void InitializeGeometry(ID3D11Device* pDevice, ModelType type );

	// set up the input layout
	void InitializeInputLayout(ID3D11Device* pDevice, const void* pBinary, size_t binarySize);

	// draw the primitive
	void Draw(ID3D11DeviceContext* pDeviceContext);


private:
	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pIndexBuffer;
	ID3D11InputLayout* pInputLayout;

	int numVerts;
	int numIndices;


};

#endif
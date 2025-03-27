//
// Paired down methods from DirectXTK for creating Vertex and Index Buffers of geometric primitives
// BGTD 9201
//


#ifndef _MODELS_H
#define _MODELS_H

#include <vector>
#include <VertexTypes.h>

using namespace DirectX;

typedef std::vector<VertexPositionNormalTexture> VertexCollection;
typedef std::vector<uint16_t> IndexCollection;

namespace Models
{
	void CreateCube(VertexCollection& vertices, IndexCollection& indices, float size);
	void CreateSphere(VertexCollection& vertices, IndexCollection& indices, float diameter, size_t tessellation);
	void CreateCylinder(VertexCollection& vertices, IndexCollection& indices, float height, float diameter, size_t tessellation);
	void CreateCone(VertexCollection& vertices, IndexCollection& indices, float diameter, float height, size_t tessellation);
	void CreateTorus(VertexCollection& vertices, IndexCollection& indices, float diameter, float thickness, size_t tessellation);
}

#endif



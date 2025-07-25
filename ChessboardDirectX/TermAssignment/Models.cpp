//
// Paired down methods from DirectXTK for creating Vertex and Index Buffers of geometric primitives
// BGTD 9201
//

#include <DirectXMath.h>
#include "Models.h"
#include <stdexcept>

namespace Models
{


	// create a cube primitive
	void CreateCube(VertexCollection& vertices, IndexCollection& indices, float size)
	{
		// A cube has six faces, each one pointing in a different direction.
		const int FaceCount = 6;

		static const XMVECTORF32 faceNormals[FaceCount] =
		{
			{ 0, 0, 1 },
			{ 0, 0, -1 },
			{ 1, 0, 0 },
			{ -1, 0, 0 },
			{ 0, 1, 0 },
			{ 0, -1, 0 },
		};

		static const XMVECTORF32 textureCoordinates[4] =
		{
			{ 1, 0 },
			{ 1, 1 },
			{ 0, 1 },
			{ 0, 0 },
		};

		size /= 2;

		// Create each face in turn.
		for (int i = 0; i < FaceCount; i++)
		{
			XMVECTOR normal = faceNormals[i];

			// Get two vectors perpendicular both to the face normal and to each other.
			XMVECTOR basis = (i >= 4) ? g_XMIdentityR2 : g_XMIdentityR1;

			XMVECTOR side1 = XMVector3Cross(normal, basis);
			XMVECTOR side2 = XMVector3Cross(normal, side1);

			// Six indices (two triangles) per face.
			size_t vbase = vertices.size();
			indices.push_back(vbase + 0);
			indices.push_back(vbase + 1);
			indices.push_back(vbase + 2);

			indices.push_back(vbase + 0);
			indices.push_back(vbase + 2);
			indices.push_back(vbase + 3);

			// Four vertices per face.
			vertices.push_back(VertexPositionNormalTexture((normal - side1 - side2) * size, normal, textureCoordinates[0]));
			vertices.push_back(VertexPositionNormalTexture((normal - side1 + side2) * size, normal, textureCoordinates[1]));
			vertices.push_back(VertexPositionNormalTexture((normal + side1 + side2) * size, normal, textureCoordinates[2]));
			vertices.push_back(VertexPositionNormalTexture((normal + side1 - side2) * size, normal, textureCoordinates[3]));
		}
	}

	// Create a sphere primitive
	void CreateSphere(VertexCollection& vertices, IndexCollection& indices, float diameter, size_t tessellation)
	{
		if (tessellation < 3)
			throw std::out_of_range("tesselation parameter out of range");

		size_t verticalSegments = tessellation;
		size_t horizontalSegments = tessellation * 2;

		float radius = diameter / 2;

		// Create rings of vertices at progressively higher latitudes.
		for (size_t i = 0; i <= verticalSegments; i++)
		{
			float v = 1 - (float)i / verticalSegments;

			float latitude = (i * XM_PI / verticalSegments) - XM_PIDIV2;
			float dy, dxz;

			XMScalarSinCos(&dy, &dxz, latitude);

			// Create a single ring of vertices at this latitude.
			for (size_t j = 0; j <= horizontalSegments; j++)
			{
				float u = (float)j / horizontalSegments;

				float longitude = j * XM_2PI / horizontalSegments;
				float dx, dz;

				XMScalarSinCos(&dx, &dz, longitude);

				dx *= dxz;
				dz *= dxz;

				XMVECTOR normal = XMVectorSet(dx, dy, dz, 0);
				XMVECTOR textureCoordinate = XMVectorSet(u, v, 0, 0);

				vertices.push_back(VertexPositionNormalTexture(normal * radius, normal, textureCoordinate));
			}
		}

		// Fill the index buffer with triangles joining each pair of latitude rings.
		size_t stride = horizontalSegments + 1;

		for (size_t i = 0; i < verticalSegments; i++)
		{
			for (size_t j = 0; j <= horizontalSegments; j++)
			{
				size_t nextI = i + 1;
				size_t nextJ = (j + 1) % stride;

				indices.push_back(i * stride + j);
				indices.push_back(nextI * stride + j);
				indices.push_back(i * stride + nextJ);

				indices.push_back(i * stride + nextJ);
				indices.push_back(nextI * stride + j);
				indices.push_back(nextI * stride + nextJ);
			}
		}
	}

	// Helper computes a point on a unit circle, aligned to the x/z plane and centered on the origin.
	static inline XMVECTOR GetCircleVector(size_t i, size_t tessellation)
	{
		float angle = i * XM_2PI / tessellation;
		float dx, dz;

		XMScalarSinCos(&dx, &dz, angle);

		XMVECTORF32 v = { dx, 0, dz, 0 };
		return v;
	}

	static inline XMVECTOR GetCircleTangent(size_t i, size_t tessellation)
	{
		float angle = (i * XM_2PI / tessellation) + XM_PIDIV2;
		float dx, dz;

		XMScalarSinCos(&dx, &dz, angle);

		XMVECTORF32 v = { dx, 0, dz, 0 };
		return v;
	}


	// Helper creates a triangle fan to close the end of a cylinder / cone
	static void CreateCylinderCap(VertexCollection& vertices, IndexCollection& indices, size_t tessellation, float height, float radius, bool isTop)
	{
		// Create cap indices.
		for (size_t i = 0; i < tessellation - 2; i++)
		{
			size_t i1 = (i + 1) % tessellation;
			size_t i2 = (i + 2) % tessellation;

			if (isTop)
			{
				std::swap(i1, i2);
			}

			size_t vbase = vertices.size();
			indices.push_back(vbase);
			indices.push_back(vbase + i1);
			indices.push_back(vbase + i2);
		}

		// Which end of the cylinder is this?
		XMVECTOR normal = g_XMIdentityR1;
		XMVECTOR textureScale = g_XMNegativeOneHalf;

		if (!isTop)
		{
			normal = -normal;
			textureScale *= g_XMNegateX;
		}

		// Create cap vertices.
		for (size_t i = 0; i < tessellation; i++)
		{
			XMVECTOR circleVector = GetCircleVector(i, tessellation);

			XMVECTOR position = (circleVector * radius) + (normal * height);

			XMVECTOR textureCoordinate = XMVectorMultiplyAdd(XMVectorSwizzle<0, 2, 3, 3>(circleVector), textureScale, g_XMOneHalf);

			vertices.push_back(VertexPositionNormalTexture(position, normal, textureCoordinate));
		}
	}


	// Creates a cylinder primitive.
	void CreateCylinder(VertexCollection& vertices, IndexCollection& indices, float height, float diameter, size_t tessellation)
	{
		if (tessellation < 3)
			throw std::out_of_range("tesselation parameter out of range");

		height /= 2;

		XMVECTOR topOffset = g_XMIdentityR1 * height;

		float radius = diameter / 2;
		size_t stride = tessellation + 1;

		// Create a ring of triangles around the outside of the cylinder.
		for (size_t i = 0; i <= tessellation; i++)
		{
			XMVECTOR normal = GetCircleVector(i, tessellation);

			XMVECTOR sideOffset = normal * radius;

			float u = (float)i / tessellation;

			XMVECTOR textureCoordinate = XMLoadFloat(&u);

			vertices.push_back(VertexPositionNormalTexture(sideOffset + topOffset, normal, textureCoordinate));
			vertices.push_back(VertexPositionNormalTexture(sideOffset - topOffset, normal, textureCoordinate + g_XMIdentityR1));

			indices.push_back(i * 2);
			indices.push_back((i * 2 + 2) % (stride * 2));
			indices.push_back(i * 2 + 1);

			indices.push_back(i * 2 + 1);
			indices.push_back((i * 2 + 2) % (stride * 2));
			indices.push_back((i * 2 + 3) % (stride * 2));
		}

		// Create flat triangle fan caps to seal the top and bottom.
		CreateCylinderCap(vertices, indices, tessellation, height, radius, true);
		CreateCylinderCap(vertices, indices, tessellation, height, radius, false);
	}



	// Creates a cone primitive.
	void CreateCone(VertexCollection& vertices, IndexCollection& indices, float diameter, float height, size_t tessellation)
	{

		if (tessellation < 3)
			throw std::out_of_range("tesselation parameter out of range");

		height /= 2;

		XMVECTOR topOffset = g_XMIdentityR1 * height;

		float radius = diameter / 2;
		size_t stride = tessellation + 1;

		// Create a ring of triangles around the outside of the cone.
		for (size_t i = 0; i <= tessellation; i++)
		{
			XMVECTOR circlevec = GetCircleVector(i, tessellation);

			XMVECTOR sideOffset = circlevec * radius;

			float u = (float)i / tessellation;

			XMVECTOR textureCoordinate = XMLoadFloat(&u);

			XMVECTOR pt = sideOffset - topOffset;

			XMVECTOR normal = XMVector3Cross(GetCircleTangent(i, tessellation), topOffset - pt);
			normal = XMVector3Normalize(normal);

			// Duplicate the top vertex for distinct normals
			vertices.push_back(VertexPositionNormalTexture(topOffset, normal, g_XMZero));
			vertices.push_back(VertexPositionNormalTexture(pt, normal, textureCoordinate + g_XMIdentityR1));

			indices.push_back(i * 2);
			indices.push_back((i * 2 + 3) % (stride * 2));
			indices.push_back((i * 2 + 1) % (stride * 2));
		}

		// Create flat triangle fan caps to seal the bottom.
		CreateCylinderCap(vertices, indices, tessellation, height, radius, false);
	}


	//--------------------------------------------------------------------------------------
	// Torus
	//--------------------------------------------------------------------------------------

	// Creates a torus primitive.
	void CreateTorus(VertexCollection& vertices, IndexCollection& indices, float diameter, float thickness, size_t tessellation)
	{
		if (tessellation < 3)
			throw std::out_of_range("tesselation parameter out of range");

		size_t stride = tessellation + 1;

		// First we loop around the main ring of the torus.
		for (size_t i = 0; i <= tessellation; i++)
		{
			float u = (float)i / tessellation;

			float outerAngle = i * XM_2PI / tessellation - XM_PIDIV2;

			// Create a transform matrix that will align geometry to
			// slice perpendicularly though the current ring position.
			XMMATRIX transform = XMMatrixTranslation(diameter / 2, 0, 0) * XMMatrixRotationY(outerAngle);

			// Now we loop along the other axis, around the side of the tube.
			for (size_t j = 0; j <= tessellation; j++)
			{
				float v = 1 - (float)j / tessellation;

				float innerAngle = j * XM_2PI / tessellation + XM_PI;
				float dx, dy;

				XMScalarSinCos(&dy, &dx, innerAngle);

				// Create a vertex.
				XMVECTOR normal = XMVectorSet(dx, dy, 0, 0);
				XMVECTOR position = normal * thickness / 2;
				XMVECTOR textureCoordinate = XMVectorSet(u, v, 0, 0);

				position = XMVector3Transform(position, transform);
				normal = XMVector3TransformNormal(normal, transform);

				vertices.push_back(VertexPositionNormalTexture(position, normal, textureCoordinate));

				// And create indices for two triangles.
				size_t nextI = (i + 1) % stride;
				size_t nextJ = (j + 1) % stride;

				indices.push_back(i * stride + j);
				indices.push_back(i * stride + nextJ);
				indices.push_back(nextI * stride + j);

				indices.push_back(i * stride + nextJ);
				indices.push_back(nextI * stride + nextJ);
				indices.push_back(nextI * stride + j);
			}
		}
	}

}
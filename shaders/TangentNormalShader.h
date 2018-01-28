#pragma once

#include <algorithm>

#include "Shader.h"
#include "../types/Matrix.h"

class TangentNormalShader : public Shader {
public:
	// uniforms for the entire shader

	Mesh *mesh;
	Matrix4f transform;

	Vector3f vertex(int faceIndex, int vertexIndex) override final {
		const FaceVector &faces = mesh->getFace(faceIndex);

		// diffuse texture coordinates
		uvs[vertexIndex] = mesh->getDiffuseTextureCoordinate(faces[vertexIndex].y);

		// vertex position
		const Vector3f vertex = mesh->getVertex(faces[vertexIndex].x);
		return MatrixVectorf::vectorFromHomogeneousMatrix(transform*Matrix4f::homogeneousMatrixfromVector(vertex));
	}

	RGBA fragment(const Vector3f &barycentric) override final {
		Vector3f uvInterpolated;
		uvInterpolated.x = uvs[0].x *barycentric.x + uvs[1].x * barycentric.y + uvs[2].x * barycentric.z;
		uvInterpolated.y = uvs[0].y *barycentric.x + uvs[1].y * barycentric.y + uvs[2].y * barycentric.z;

		return mesh->getNormalAsColour(uvInterpolated);
	}

	ShaderType getType() override final { return ShaderType::TANGENT_NORMAL; }

private:
	Vector3f uvs[3];
};
#pragma once

#include "Shader.h"
#include "../types/Matrix.h"

class FaceIlluminationShader : public Shader {
public:
	// uniforms for the entire shader
	Matrix4f transform;
	Vector3f lightDirection;
	Mesh *mesh;

	Vector3f vertex(int faceIndex, int vertexIndex) override final {
		// vertex position
		const FaceVector &faces = mesh->getFace(faceIndex);
		const Vector3f vertex = mesh->getVertex(faces[vertexIndex].x);
		return MatrixVectorf::createFromHomogeneousMatrix(transform*Matrix4f::fromVector(vertex));
	}

	void geometry(int faceIndex, Vector3f vertices[3]) override final {
		// face normal
		Vector3f n = (vertices[1] - vertices[0]) ^ (vertices[2] - vertices[0]);
		n.normalize();
		faceIlumination = n.dot(lightDirection);

		// diffuse texture coordinate (one per primitive)
		const FaceVector &faces = mesh->getFace(faceIndex);
		uv = mesh->getDiffuseTextureCoordinate(faces[0].y);
	}

	RGBA fragment(const Vector3f &barycentric) override final {
		RGBA colour = mesh->getDiffuseColor(uv);
		colour.applyLightIntensity(faceIlumination);
		return colour;
	}

	ShaderType getType() override final {
		return ShaderType::FACE_ILLUMINATION;
	}

private:
	float faceIlumination;
	Vector3f uv;
};
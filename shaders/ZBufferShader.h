#pragma once

#include "Shader.h"

class ZBufferShader : public Shader {
public:
	// uniforms for the entire shader
	Matrix4f transform;
	Mesh *mesh;
	float * zBuffer;
	float depth;
	int screenWidth;

	Vector3f vertex(int faceIndex, int vertexIndex) override final {
		// vertex position
		const FaceVector &faces = mesh->getFace(faceIndex);
		const Vector3f vertex = mesh->getVertex(faces[vertexIndex].x);
		return MatrixVectorf::createFromHomogeneousMatrix(transform*Matrix4f::fromVector(vertex));
	}

	RGBA fragment(const Vector3f &barycentric) override final {
		RGBA colour = WHITE;
		colour.applyLightIntensity(zBuffer[FRAGMENT_COORDINATES.x + FRAGMENT_COORDINATES.y * screenWidth] / depth);
		return colour;
	}

	ShaderType getType() override final {
		return ShaderType::ZBUFFER;
	}
};
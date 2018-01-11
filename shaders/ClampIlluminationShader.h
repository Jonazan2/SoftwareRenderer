#pragma once

#include <algorithm>

#include "Shader.h"
#include "../types/Matrix.h"

class ClampIlluminationShader : public Shader {
public:
	// uniforms for the entire shader
	Matrix4f transform;
	Vector3f lightDirection;
	Mesh *mesh;

	Vector3f vertex(int faceIndex, int vertexIndex) override final {
		const FaceVector &faces = mesh->getFace(faceIndex);

		// diffuse texture coordinates
		uvs[vertexIndex] = mesh->getDiffuseTextureCoordinate(faces[vertexIndex].y);

		// calculate light intensity per vertex
		Vector3f n = mesh->getNormal(faces[vertexIndex].z);
		n.normalize();
		light[vertexIndex] = n.dot(lightDirection);

		// vertex position
		const Vector3f vertex = mesh->getVertex(faces[vertexIndex].x);
		return MatrixVectorf::createFromHomogeneousMatrix(transform*Matrix4f::fromVector(vertex));
	}


	RGBA fragment(const Vector3f &barycentric) override final {
		Vector2i uvInterpolated;
		uvInterpolated.x = uvs[0].x *barycentric.x + uvs[1].x * barycentric.y + uvs[2].x * barycentric.z;
		uvInterpolated.y = uvs[0].y *barycentric.x + uvs[1].y * barycentric.y + uvs[2].y * barycentric.z;

		float lightIntensity = light[0] * barycentric.x + light[1] * barycentric.y + light[2] * barycentric.z;

		if (lightIntensity > 0.8f) {
			lightIntensity = 0.8f;
		} else if (lightIntensity > 0.6f) {
			lightIntensity = 0.6f;
		} else if (lightIntensity > 0.2f) {
			lightIntensity = 0.2f;
		} else {
			lightIntensity = 0.0f;
		}

		RGBA colour = mesh->getTextureColor(uvInterpolated);
		colour.applyLightIntensity(std::max(0.0f, lightIntensity));
		return colour;
	}

	ShaderType getType() override final {
		return ShaderType::CLAMP_ILUMINATION;
	}

private:
	Vector3f uvs[3];
	float light[3];
};
#pragma once

#include "Shader.h"
#include <algorithm>

class PhongShader : public Shader {
public:
	// uniforms for the entire shader
	Vector3f lightDirection;
	Mesh *mesh;

	Matrix4f MWP;
	Matrix4f MWPInversedTransposed;
	Matrix4f transform;

	Vector3f vertex(int faceIndex, int vertexIndex) override final {
		const FaceVector &faces = mesh->getFace(faceIndex);

		// diffuse texture coordinates
		uvs[vertexIndex] = mesh->getDiffuseTextureCoordinate(faces[vertexIndex].y);

		// Transform normals and light
		normals[vertexIndex] = MatrixVectorf::createFromHomogeneousMatrix(MWPInversedTransposed * Matrix4f::fromVector(mesh->getNormal(faces[vertexIndex].z)));

		// vertex position
		const Vector3f vertex = mesh->getVertex(faces[vertexIndex].x);
		ndc[vertexIndex] = MatrixVectorf::createFromHomogeneousMatrix(MWP*Matrix4f::fromVector(vertex));
		return MatrixVectorf::createFromHomogeneousMatrix(transform*Matrix4f::fromVector(vertex));
	}

	RGBA fragment(const Vector3f &barycentric) override final {
		Vector3f uvInterpolated;
		uvInterpolated.x = uvs[0].x *barycentric.x + uvs[1].x * barycentric.y + uvs[2].x * barycentric.z;
		uvInterpolated.y = uvs[0].y *barycentric.x + uvs[1].y * barycentric.y + uvs[2].y * barycentric.z;

		Vector3f normalInterpolated;
		normalInterpolated.x = normals[0].x *barycentric.x + normals[1].x * barycentric.y + normals[2].x * barycentric.z;
		normalInterpolated.y = normals[0].y *barycentric.x + normals[1].y * barycentric.y + normals[2].y * barycentric.z;
		normalInterpolated.z = normals[0].z *barycentric.x + normals[1].z * barycentric.y + normals[2].z * barycentric.z;
		normalInterpolated.normalize();

		// convert object space to tangent space
		Vector3f row0 = ndc[1] - ndc[0];
		Vector3f row1 = ndc[2] - ndc[0];
		Matrix3f A = {
			{ row0.x, row0.y, row0.z },
			{ row1.x, row1.y, row1.z },
			{ normalInterpolated.x, normalInterpolated.y, normalInterpolated.z }
		};
		Matrix3f AI = A.invert();

		Matrix<float, 3, 1> iM = AI * Matrix3f::createFromVector(Vector3f(uvs[1][0] - uvs[0][0], uvs[2][0] - uvs[0][0], 0));
		Vector3f i(iM[0][0], iM[1][0], iM[2][0]);
		i.normalize();

		Matrix<float, 3, 1> jM = AI * Matrix3f::createFromVector(Vector3f(uvs[1][1] - uvs[0][1], uvs[2][1] - uvs[0][1], 0));
		Vector3f j(jM[0][0], jM[1][0], jM[2][0]);
		j.normalize();

		Matrix<float, 3, 3> B = {
			{ i[0], j[0], normalInterpolated.x },
			{ i[1], j[1], normalInterpolated.y },
			{ i[2], j[2], normalInterpolated.z }
		};

		Vector3f normalMap = mesh->getNormalFromMap(uvInterpolated);
		Matrix<float, 3, 1> n = B *Matrix4f::createFromVector(normalMap);
		Vector3f normal(n[0][0], n[1][0], n[2][0]);
		normal.normalize();

		float diffuseLight = normal.dot(lightDirection);
		Vector3f reflectedLight = normal * (2.0f * (normal.dot(lightDirection))) - lightDirection;
		reflectedLight.normalize();
		float specular = pow(std::max(reflectedLight.z, 0.0f), mesh->getSpecularIntensity(uvInterpolated));

		RGBA colour = mesh->getDiffuseColor(uvInterpolated);
		colour.applyLightIntensity(std::max(0.0f, (diffuseLight + 0.08f * specular)));
		return colour;
	}

	ShaderType getType() override final { return ShaderType::PHONG; }

private:
	Vector3f uvs[3];
	Vector3f ndc[3];
	Vector3f normals[3];
};
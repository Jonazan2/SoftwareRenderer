#pragma once

#include <string>
#include <vector>
#include "../types/Vector3.h"
#include "../types/Types.h"
#include "../types/Matrix.h"

using FaceVector = std::vector<Vector3i>;

class Mesh {

public:
	Mesh();
	~Mesh();

	void loadObjFromFile(const std::string& path);
	void loadDiffuseTexture(const std::string& path);
	void loadNormalMap(const std::string& path);
	void loadSpecularMap(const std::string& path);

	int getVerticesCount() const;
	int getTextureCoordinatesCount() const;
	int getNormalsCount() const;
	int getFacesCount() const;
	const std::vector<FaceVector>& getFaces() const;

	const Vector3f& getVertex(size_t index) const;
	Vector3f getDiffuseTextureCoordinate(size_t index) const;
	const Vector3f& getNormal(size_t index) const;
	const FaceVector& getFace(size_t index) const;

	RGBA getDiffuseColor(const Vector3f &textureCoordinate) const;
	Vector3f getNormalFromMap(const Vector3f &textureCoordinate) const;
	RGBA getNormalAsColour(const Vector3f &textureCoordinate) const;
	float getSpecularIntensity(const Vector3f &textureCoordinate) const;

	const Matrix4f& getModelMatrix() const { return model; }
	void translate(Vector3f translation);
	void scale(float x, float y, float z);
	void rotatePitch(float degrees);
	void rotateRoll(float degrees);
	void rotateYaw(float degrees);

private:

	struct Texture {
		int width;
		int height;
		int pitch;
		byte *data;
	};
	
	Matrix4f model;
	Texture diffuse;
	Texture normalMap;
	Texture specularMap;

	std::vector<Vector3f> vertices;
	std::vector<Vector3f> textureCoordinates;
	std::vector<Vector3f> normals;
	std::vector<FaceVector> faces;
	std::vector<Vector3f> spaceVertices;

	void loadTexture(Texture &texture, const std::string &filename, int format);
};
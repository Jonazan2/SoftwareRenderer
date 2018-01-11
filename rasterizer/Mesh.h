#pragma once

#include <string>
#include <vector>
#include "../types/Vector3.h"
#include "../types/Types.h"

using FaceVector = std::vector<Vector3i>;

class Mesh {

public:
	Mesh();
	~Mesh();

	void loadObjFromFile(const std::string& path);
	void loadDiffuseTexture(const std::string& path);

	int getVerticesCount() const;
	int getTextureCoordinatesCount() const;
	int getNormalsCount() const;
	int getFacesCount() const;
	const std::vector<FaceVector>& getFaces() const;

	const Vector3f& getVertex(size_t index) const;
	Vector3f getDiffuseTextureCoordinate(size_t index) const;
	const Vector3f& getNormal(size_t index) const;
	const FaceVector& getFace(size_t index) const;

	RGBA Mesh::getTextureColor(const Vector2i &textureCoordinate) const;

private:

	struct Texture {
		int width;
		int height;
		byte *data;
	};

	std::vector<Vector3f> vertices;
	std::vector<Vector3f> textureCoordinates;
	std::vector<Vector3f> normals;
	std::vector<FaceVector> faces;
	std::vector<Vector3f> spaceVertices;
	Texture diffuse;
};
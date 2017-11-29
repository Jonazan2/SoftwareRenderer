#pragma once

#include <string>
#include <vector>
#include "Vector3.h"

using FaceVector = std::vector<Vector3i>;

class Mesh {

public:
	Mesh();

	void loadObjFromFile(const std::string& path);

	int getVerticesCount() const;
	int getTextureCoordinatesCount() const;
	int getNormalsCount() const;
	const std::vector<FaceVector>& getFaces() const;

	const Vector3f& getVertex(size_t index) const;
	const Vector3f& getTextureCoordinate(size_t index) const;
	const Vector3f& getNormal(size_t index) const;


private:
	std::vector<Vector3f> vertices;
	std::vector<Vector3f> textureCoordinates;
	std::vector<Vector3f> normals;
	std::vector<FaceVector> faces;
	std::vector<Vector3f> spaceVertices;
};
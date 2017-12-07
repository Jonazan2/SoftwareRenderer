#include "Mesh.h"

#include <fstream>
#include <sstream>
#include <iostream>

Mesh::Mesh() {
	vertices.clear();
	textureCoordinates.clear();
	normals.clear();
	faces.clear();
	spaceVertices.clear();
}

void Mesh::loadObjFromFile(const std::string& path) {
	std::ifstream file(path, std::ifstream::in);

	if (file.is_open()) {

		float x, y, z;
		std::string line;
		std::string faceLine;
		std::string indexValues;
		int faceComponentIndex;
		std::istringstream faceStream;
		std::istringstream in;
		Vector3i face;

		while (std::getline(file, line)) {
			in.clear();
			in.str(line);

			if (line[0] != '#') {
				x = 0, y = 0, z = 0;

				std::string type;
				in >> type;

				if (type == "v") {
					in >> x >> y >> z;
					vertices.emplace_back(x,y,z);
				} else if (type == "vt") {
					in >> x >> y;
					textureCoordinates.emplace_back(x, y, 0);
				} else if (type == "vn") {
					in >> x >> y >> z;
					normals.emplace_back(x, y, z);
				} else if (type == "f") {
					FaceVector vector;
					while (std::getline(in, faceLine, ' ')) {
						if (!faceLine.empty()) {
							faceStream.clear();
							faceStream.str(faceLine);

							face = { 0,0,0 };
							faceComponentIndex = 0;
							while (std::getline(faceStream, indexValues, '/')) {
								face[faceComponentIndex] = atoi(indexValues.c_str()) - 1;
								++faceComponentIndex;
							}
							vector.push_back(face);
						}
					}
					faces.push_back(vector);
				}
			}
		}
	}

	file.close();
}

int Mesh::getVerticesCount() const {
	return vertices.size();
}

int Mesh::getTextureCoordinatesCount() const {
	return textureCoordinates.size();
}

int Mesh::getNormalsCount() const {
	return normals.size();
}

int Mesh::getFacesCount() const {
	return faces.size();
}

const std::vector<FaceVector>& Mesh::getFaces() const {
	return faces;
}

const Vector3f& Mesh::getTextureCoordinate(size_t index) const {
	assert(index < textureCoordinates.size());
	return textureCoordinates[index];
}

const Vector3f& Mesh::getVertex(size_t index) const {
	assert(index < vertices.size());
	return vertices[index];
}

const Vector3f& Mesh::getNormal(size_t index) const {
	assert(index < normals.size());
	return normals[index];
}

const FaceVector& Mesh::getFace(size_t index) const {
	assert(index < faces.size());
	return faces[index];
}
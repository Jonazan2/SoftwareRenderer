#include "Mesh.h"

#include <fstream>
#include <sstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Mesh::Mesh() {
	vertices.clear();
	textureCoordinates.clear();
	normals.clear();
	faces.clear();
	spaceVertices.clear();
}

Mesh::~Mesh() {
	if (texture.data != nullptr) {
		stbi_image_free(texture.data);
	}
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

void Mesh::loadTexture(const std::string& path) {
	int req_format = STBI_rgb_alpha;
	int width, height, orig_format;
	byte *textureData = stbi_load(path.c_str(), &width, &height, &orig_format, req_format);
	assert(textureData != nullptr && req_format == orig_format);

	texture = { width, height, textureData };
}

RGBA Mesh::getTextureColor(const Vector2i &textureCoordinate) {
	assert(texture.data != nullptr);
	// we must take into account the pitch: 4, and rotate vertically to match the origin of SDL
	int index = ((textureCoordinate.x * 4) + ((texture.height - textureCoordinate.y) * texture.height * 4));
	RGBA colour;
	colour.red = texture.data[index];
	colour.green = texture.data[index + 1];
	colour.blue = texture.data[index + 2];
	colour.alpha = texture.data[index + 3];
	return colour;
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

Vector3f Mesh::getTextureCoordinate(size_t index) const {
	assert(index < textureCoordinates.size());
	Vector3f uv = textureCoordinates[index];
	uv.x = uv.x * texture.width;
	uv.y = uv.y * texture.height;
	return uv;
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
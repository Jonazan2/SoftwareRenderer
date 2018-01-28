#include "Mesh.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Mesh::Mesh() : model(Matrix4f::identity()) {
	vertices.clear();
	textureCoordinates.clear();
	normals.clear();
	faces.clear();
	spaceVertices.clear();
}

Mesh::~Mesh() {
	if (diffuse.data != nullptr) {
		stbi_image_free(diffuse.data);
	}
	if (specularMap.data != nullptr) {
		stbi_image_free(specularMap.data);
	}
	if (normalMap.data != nullptr) {
		stbi_image_free(normalMap.data);
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

void Mesh::loadDiffuseTexture(const std::string& path) {
	loadTexture(diffuse, path, STBI_rgb_alpha);
}

void Mesh::loadNormalMap(const std::string& path) {
	loadTexture(normalMap, path, STBI_rgb_alpha);
}

void Mesh::loadSpecularMap(const std::string& path) {
	loadTexture(specularMap, path, STBI_rgb_alpha);
}

void Mesh::loadTexture(Texture &texture, const std::string &filename, int format) {
	stbi_set_flip_vertically_on_load(true);

	int width, height, orig_format;
	byte *textureData = stbi_load(filename.c_str(), &width, &height, &orig_format, format);
	assert(textureData != nullptr);

	texture = { width, height, orig_format, textureData };
}

RGBA Mesh::getDiffuseColor(const Vector3f &textureCoordinate) const {
	assert(diffuse.data != nullptr);
	Vector2i uv( textureCoordinate.x * diffuse.width , textureCoordinate.y * diffuse.height );
	int index = ((uv.x * diffuse.pitch) + (uv.y * diffuse.height * diffuse.pitch));
	RGBA colour;
	colour.red = diffuse.data[index];
	colour.green = diffuse.data[index + 1];
	colour.blue = diffuse.data[index + 2];
	colour.alpha = diffuse.data[index + 3];
	return colour;
}

Vector3f Mesh::getNormalFromMap(const Vector3f &textureCoordinate) const {
	assert(normalMap.data != nullptr);
	Vector2i uv(textureCoordinate.x * normalMap.width, textureCoordinate.y * normalMap.height);
	int index = ((uv.x * normalMap.pitch) + (uv.y * normalMap.height * normalMap.pitch));
	Vector3f normal;
	for (int i = 0; i < 3; i++, index++) {
		normal[i] = (((float)normalMap.data[index] / 255.f) * 2.0f) - 1.f;
	}
	return normal;
}

float Mesh::getSpecularIntensity(const Vector3f &textureCoordinate) const {
	assert(specularMap.data != nullptr);
	Vector2i uv(textureCoordinate.x * specularMap.width, textureCoordinate.y * specularMap.height);
	int index = ((textureCoordinate.x * specularMap.pitch) + (textureCoordinate.y * specularMap.height * specularMap.pitch));
	return specularMap.data[index] / 1.0f;
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

Vector3f Mesh::getDiffuseTextureCoordinate(size_t index) const {
	assert(index < textureCoordinates.size());
	Vector3f uv = textureCoordinates[index];
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

void Mesh::translate(Vector3f translation) {
	Matrix4f translationMatrix = {
		{ 1,0,0,translation.x },
		{ 0,1,0,translation.y },
		{ 0,0,1,translation.z },
		{ 0,0,0,1 }
	};
	model = model * translationMatrix;
}

void Mesh::scale(float x, float y, float z) {
	Matrix4f scaleMatrix = {
		{ x,0,0,0 },
		{ 0,y,0,0 },
		{ 0,0,z,0 },
		{ 0,0,0,1 }
	};
	model = model * scaleMatrix;
}

void Mesh::rotatePitch(float degrees) {
	// rotate x axis
	Matrix4f rotateXMatrix = {
		{ 1,0,0,0 },
		{ 0,cos(degrees),-sin(degrees),0 },
		{ 0,sin(degrees),cos(degrees),0 },
		{ 0,0,0,1 }
	};
	model = model * rotateXMatrix;
}

void Mesh::rotateRoll(float degrees) {
	// rotate z axis
	Matrix4f rotateZMatrix = {
		{ cos(degrees),-sin(degrees),0,0 },
		{ sin(degrees),cos(degrees),0,0 },
		{ 0,0,1,0 },
		{ 0,0,0,1 }
	};
	model = model * rotateZMatrix;
}

void Mesh::rotateYaw(float degrees) {
	// rotate y axis
	Matrix4f rotateYMatrix = {
		{ cos(degrees),0,sin(degrees),0 },
		{ 0,1,0,0 },
		{ -sin(degrees),0,cos(degrees),0 },
		{ 0,0,0,1 }
	};
	model = model * rotateYMatrix;
}
#pragma once

#include "../types/Types.h"
#include "../rasterizer/Mesh.h"

enum class ShaderType : int {
	FACE_ILLUMINATION = 0,
	GOURAUD,
	CLAMP_ILUMINATION,
	ZBUFFER,
	PHONG,
	TANGENT_NORMAL
};

class Shader {
public:
	Vector2i FRAGMENT_COORDINATES;

	virtual Vector3f vertex(int faceIndex, int vertexIndex) = 0;
	virtual void geometry(int faceIndex, Vector3f vertices[3]) {};
	virtual RGBA fragment(const Vector3f &barycentric) = 0;
	virtual ShaderType getType() = 0;
};
#pragma once

#include <SDL.h>

#include "Mesh.h"
#include "../types/Types.h"
#include "Camera.h"


class Rasterizer {
public:
	Rasterizer(Mesh *mesh, const Camera& camera) : mesh(mesh), camera(camera) {
		clearBuffers();
	};

	~Rasterizer();
	void clearBuffers();

	void createWindow();
	void createViewportMatrix();
	void createProjectionMatrix();
	void draw();

	Camera& getCamera() { return camera; }
	void setLightPosition(const Vector3f light) { this->light = light; }

private:

	static const int SCREEN_WIDTH = 1024;
	static const int SCREEN_HEIGHT = 768;

	Mesh *mesh;
	Camera camera;
	Vector3f light;
	Matrix4f viewport;
	Matrix4f projection;

	SDL_Window* window;
	SDL_Texture* texture;
	SDL_Renderer* renderer;

	RGBA frameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
	float zBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

	void plotPixel(int x, int y, RGBA colour);
	void drawLine(int x0, int y0, int x1, int y1, RGBA colour);
	void drawTriangle(Vector3f vertices[3], Vector3f uvs[3], Vector3f normals[3], Mesh &mesh);
	bool isDegenerate(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2);
	BoundingBox calculateBoundingBoxOfTriangle(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2);
	bool isPointInsideTriangle(const Vector3f &barycentricCoordinates);
	Vector3f calculateBarycentricCoordinates(const Vector2i &point, const Vector3f &v0, const Vector3f &v1, const Vector3f &v2);
	bool passZBufferTest(const Vector2i &point, const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &barycentricCoordinates);
	void drawBoundingBox(const BoundingBox &box, const RGBA &colour);
	Vector3f createFromHomogeneousMatrix(const MatrixVectorf &m);
	Vector2i interpolateTextureCoordinates(const Vector3f &barycentric, const Vector3f &uv0, const Vector3f &uv1, const Vector3f &uv2);
	float applyGouraudInterpolation(const Vector3f &barycentric, float intensityN0, float intensityN1, float intensityN2);
};
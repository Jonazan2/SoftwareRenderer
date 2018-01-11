#pragma once

#include <SDL.h>
#include <memory>

#include "Mesh.h"
#include "../types/Types.h"
#include "Camera.h"
#include "../shaders/Shader.h"

class Rasterizer {
public:

	Rasterizer(Mesh *mesh, Camera* camera);
	~Rasterizer();

	void createWindow();
	void createViewportMatrix();
	void createProjectionMatrix();
	void loadShader(std::unique_ptr<Shader> &shader) { this->shader = std::move(shader); }

	void draw();
	void clearBuffers();

	void setCamera(Camera* camera) { this->camera = camera; }
	void setLightPosition(const Vector3f light) { this->light = light; }
	void setFpsCount(int fps) { SDL_SetWindowTitle(window, ("Software Renderer FPS:" + std::to_string(fps)).c_str()); }

private:

	static const int SCREEN_WIDTH = 1024;
	static const int SCREEN_HEIGHT = 768;

	Mesh *mesh;
	Camera *camera;
	std::unique_ptr<Shader> shader;
	Vector3f light;

	Matrix4f model;
	Matrix4f view;
	Matrix4f projection;
	Matrix4f viewport;
	Matrix4f transform;

	SDL_Window* window;
	SDL_Texture* texture;
	SDL_Renderer* renderer;

	RGBA *frameBuffer;
	float *zBuffer;

	void plotPixel(int x, int y, RGBA colour);
	void drawLine(int x0, int y0, int x1, int y1, RGBA colour);
	void drawTriangle(Vector3f vertices[3]);
	
	void setUniformsInShader();

	bool isDegenerate(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2);
	BoundingBox calculateBoundingBoxOfTriangle(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2);
	bool isPointInsideTriangle(const Vector3f &barycentricCoordinates);
	Vector3f calculateBarycentricCoordinates(const Vector2i &point, const Vector3f &v0, const Vector3f &v1, const Vector3f &v2);
	bool passZBufferTest(const Vector2i &point, const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &barycentricCoordinates);
	void drawBoundingBox(const BoundingBox &box, const RGBA &colour);
};
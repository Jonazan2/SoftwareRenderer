#include <stdlib.h>
#include <algorithm>
#include <assert.h>

#define SDL_MAIN_HANDLED 
#include <SDL.h>

#include "Types.h"
#include "Mesh.h"

static const int SCREEN_WIDTH = 1024;
static const int SCREEN_HEIGHT = 768;

RGBA frameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

void plotPixel(int x, int y, RGBA colour);
void drawLine(int x0, int y0, int x1, int y1, RGBA colour);
void drawTriangle(const Vector2i &v0, const Vector2i &v1, const Vector2i &v2, const RGBA& colour);
bool isDegenerate(const Vector2i &v0, const Vector2i &v1, const Vector2i &v2);
BoundingBox calculateBoundingBoxOfTriangle(const Vector2i &v0, const Vector2i &v1, const Vector2i &v2);
bool IsPointInsideTriangle(const Vector2i &point, const Vector2i &v0, const Vector2i &v1, const Vector2i &v2);
void drawBoundingBox(const BoundingBox &box, const RGBA &colour);

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		return -1;
	}

	// Load mesh from OBJ Wavefront file
	Mesh mesh;
	mesh.loadObjFromFile("head.obj");

	// Create SDL window and rendered for our frame buffer
	SDL_Window *window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Init frame buffer to BLACK
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			frameBuffer[j][i] = BLACK;
		}
	}

	SDL_Event event;
	bool quit = false;
	while (!quit) {

		// handle keyboard events
		while (SDL_PollEvent(&event)) {
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
				quit = true;
			}

			if (event.type == SDL_QUIT) {
				quit = true;
			}
		}

		// draw faces (without culling for now)
		for (int i = 0; i < mesh.getFacesCount(); i++) {
			const FaceVector& face = mesh.getFace(i);
			Vector2i screenCoordinates[3];
			for (int j = 0; j < 3; j++) {
				const Vector3f vertexCoordinate = mesh.getVertex(face[j].x);
				screenCoordinates[j] = Vector2i((vertexCoordinate.x + 1.)*SCREEN_WIDTH / 2., (vertexCoordinate.y + 1.)*SCREEN_HEIGHT / 2.);
			}
			RGBA random = { rand() % 0xFF, rand() % 0xFF , rand() % 0xFF , (byte)0xFF };
			drawTriangle(screenCoordinates[0], screenCoordinates[1], screenCoordinates[2], random);
		}

		SDL_UpdateTexture(texture, NULL, frameBuffer, SCREEN_WIDTH * sizeof(byte) * 4);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	return 0;
}

void plotPixel(int x, int y, RGBA colour) {
	assert(x < SCREEN_WIDTH && y < SCREEN_HEIGHT);
	frameBuffer[SCREEN_HEIGHT - y][x] = colour;
}

void drawLine(int x0, int y0, int x1, int y1, RGBA colour) {
	const bool steep = (fabs(y1 - y0) > fabs(x1 - x0));

	if (steep) {
		std::swap(x0, y0);
		std::swap(x1, y1);
	}

	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	const float dx = x1 - x0;
	const float dy = fabs(y1 - y0);
	float error = dx / 2.0f;

	const int ystep = (y0 < y1) ? 1 : -1;

	for (int x = x0, y = y0; x < x1; x++) {
		if (steep) {
			plotPixel(y, x, colour);
		} else {
			plotPixel(x, y, colour);
		}

		error -= dy;
		if (error < 0) {
			y += ystep;
			error += dx;
		}
	}
}

void drawTriangle(const Vector2i &v0, const Vector2i &v1, const Vector2i &v2, const RGBA& colour) {
	if (isDegenerate(v0, v1, v2)) {
		return;
	}

	BoundingBox box = calculateBoundingBoxOfTriangle(v0, v1, v2);
	for (int x = box.min.x; x <= box.max.x; x++) {
		for (int y = box.min.y; y <= box.max.y; y++) {
			if (IsPointInsideTriangle({ x, y }, v0, v1, v2)) {
				plotPixel(x, y, colour);
			}
		}
	}
}

bool isDegenerate(const Vector2i &v0, const Vector2i &v1, const Vector2i &v2) {
	return v0.y == v1.y && v0.y == v2.y;
}

BoundingBox calculateBoundingBoxOfTriangle(const Vector2i &v0, const Vector2i &v1, const Vector2i &v2) {
	BoundingBox box;
	box.min = Vector2i(std::min({ v0.x, v1.x, v2.x, SCREEN_WIDTH - 1 }),
		std::min({ v0.y, v1.y, v2.y, SCREEN_HEIGHT - 1 }));

	box.max = Vector2i(std::max({ v0.x, v1.x, v2.x}),
		std::max({ v0.y, v1.y, v2.y }));

	box.max.x = std::min(box.max.x, SCREEN_WIDTH - 1);
	box.max.y = std::min(box.max.y, SCREEN_HEIGHT - 1);

	return box;
}

bool IsPointInsideTriangle(const Vector2i &point, const Vector2i &v0, const Vector2i &v1, const Vector2i &v2) {
	Vector3f a(v2.x - v0.x, v1.x - v0.x, v0.x - point.x);
	Vector3f b(v2.y - v0.y, v1.y - v0.y, v0.y - point.y);
	Vector3f u = a.cross(b);
	if (std::abs(u.z) < 1) {
		return false;
	}

	// P = (1- u - v)v0 + uv1 + vv2
	Vector3f barycentric(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return barycentric.x >= 0 && barycentric.y >= 0 && barycentric.z >= 0;
}

void drawBoundingBox(const BoundingBox &box, const RGBA &colour) {
	drawLine(box.min.x, box.min.y, box.max.x, box.min.y, colour);
	drawLine(box.min.x, box.max.y, box.max.x, box.max.y, colour);
	drawLine(box.min.x, box.min.y, box.min.x, box.max.y, colour);
	drawLine(box.max.x, box.min.y, box.max.x, box.max.y, colour);
}
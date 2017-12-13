#include <stdlib.h>
#include <algorithm>
#include <assert.h>

#define SDL_MAIN_HANDLED 
#include <SDL.h>

#include "Types.h"
#include "Mesh.h"

static const int SCREEN_WIDTH = 1024;
static const int SCREEN_HEIGHT = 768;

static const Vector3f LIGHT_DIRECTION(0.0f, 0.0f, -0.2f);

RGBA frameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
float zBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

Vector3f world2screen(Vector3f v);
void plotPixel(int x, int y, RGBA colour);
void drawLine(int x0, int y0, int x1, int y1, RGBA colour);
void drawTriangle(Vector3f vertices[3], Vector3f uvs[3], Vector3f normals[3], Mesh &mesh);
Vector2i interpolateTextureCoordinates(const Vector3f &barycentric, const Vector3f &uv0, const Vector3f &uv1, const Vector3f &uv2);
float applyGouraudInterpolation(const Vector3f &barycentric, float intensityN0, float intensityN1, float intensityN2);

bool isDegenerate(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2);
BoundingBox calculateBoundingBoxOfTriangle(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2);
bool isPointInsideTriangle(const Vector3f &barycentricCoordinates);
Vector3f calculateBarycentricCoordinates(const Vector2i &point, const Vector3f &v0, const Vector3f &v1, const Vector3f &v2);
bool passZBufferTest(const Vector2i &point, const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &barycentricCoordinates);
void drawBoundingBox(const BoundingBox &box, const RGBA &colour);
void applyLightIntensityToColour(float intensity, RGBA &colour);

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		return -1;
	}

	// Load mesh from OBJ Wavefront file
	Mesh mesh;
	mesh.loadObjFromFile("head.obj");
	mesh.loadTexture("head_diffuse.png");

	// Create SDL window and rendered for our frame buffer
	SDL_Window *window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Init frame buffer to BLACK and zBuffer to the largest float possible ( 0 will be closest to camera )
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			frameBuffer[j][i] = BLACK;
			zBuffer[j][i] = -std::numeric_limits<float>::max();
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

		// draw faces of the mesh
		for (int i = 0; i < mesh.getFacesCount(); i++) {
			const FaceVector& face = mesh.getFace(i);
			Vector3f screenCoordinates[3];
			Vector3f texureCoordinates[3];
			Vector3f normals[3];
			Vector3f vertexCoordinates[3];
			for (int j = 0; j < 3; j++) {
				vertexCoordinates[j] = mesh.getVertex(face[j].x);
				texureCoordinates[j] = mesh.getTextureCoordinate(face[j].y);
				normals[j] = mesh.getNormal(face[j].z).getNormalizeVector();
				screenCoordinates[j] = world2screen(vertexCoordinates[j]);
			}

			// calculate face normals and check for back face culling
			Vector3f n = (vertexCoordinates[2] - vertexCoordinates[0]) ^ (vertexCoordinates[1] - vertexCoordinates[0]);
			n.normalize();
			if (n.dot(LIGHT_DIRECTION) > 0.0f) {
				drawTriangle(screenCoordinates, texureCoordinates, normals, mesh);
			}
		}

		SDL_UpdateTexture(texture, nullptr, frameBuffer, SCREEN_WIDTH * sizeof(byte) * 4);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
	}

	return 0;
}

Vector3f world2screen(Vector3f v) {
	return Vector3f(int((v.x + 1.)*SCREEN_WIDTH / 2. + .5), int((v.y + 1.)*SCREEN_HEIGHT / 2. + .5), v.z);
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

void drawTriangle(Vector3f vertices[3], Vector3f uvs[3], Vector3f normals[3], Mesh &mesh) {
	// check if triangle is degenerate to discard it
	if (isDegenerate(vertices[0], vertices[1], vertices[2])) {
		return;
	}

	// pre calculculate light intensity at normals
	float intensities[3];
	for (int i = 0; i < 3; i++) {
		intensities[i] = normals[i].dot(LIGHT_DIRECTION);
	}

	// draw triangle
	BoundingBox box = calculateBoundingBoxOfTriangle(vertices[0], vertices[1], vertices[2]);
	for (int x = box.min.x; x <= box.max.x; x++) {
		for (int y = box.min.y; y <= box.max.y; y++) {
			Vector2i point = { x, y };
			Vector3f barycentric = calculateBarycentricCoordinates(point, vertices[0], vertices[1], vertices[2]);
			if (isPointInsideTriangle(barycentric) && passZBufferTest(point, vertices[0], vertices[1], vertices[2], barycentric)) {
				Vector2i uv = interpolateTextureCoordinates(barycentric, uvs[0], uvs[1], uvs[2]);
				RGBA colour = mesh.getTextureColor(uv);
				float lightIntensity = applyGouraudInterpolation(barycentric, intensities[0], intensities[1], intensities[2]);
				applyLightIntensityToColour(lightIntensity, colour);
				plotPixel(x, y, colour);
			}
		}
	}
}

bool isDegenerate(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2) {
	return v0.y == v1.y && v0.y == v2.y;
}

BoundingBox calculateBoundingBoxOfTriangle(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2) {
	BoundingBox box;
	box.min = Vector2i(std::min({ v0.x, v1.x, v2.x, static_cast<float>(SCREEN_WIDTH - 1) }),
		std::min({ v0.y, v1.y, v2.y, static_cast<float>(SCREEN_HEIGHT - 1) }));

	box.max = Vector2i(std::max({ v0.x, v1.x, v2.x}),
		std::max({ v0.y, v1.y, v2.y }));

	box.max.x = std::min(box.max.x, SCREEN_WIDTH - 1);
	box.max.y = std::min(box.max.y, SCREEN_HEIGHT - 1);

	return box;
}

Vector3f calculateBarycentricCoordinates(const Vector2i &point, const Vector3f &v0, const Vector3f &v1, const Vector3f &v2) {
	Vector3f a(v2.x - v0.x, v1.x - v0.x, v0.x - point.x);
	Vector3f b(v2.y - v0.y, v1.y - v0.y, v0.y - point.y);
	Vector3f u = a.cross(b);
	if (u.z == 0) {
		return Vector3f(-1, 1, 1);
	}
	return Vector3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

bool isPointInsideTriangle(const Vector3f &barycentricCoordinates) {
	return barycentricCoordinates.x >= 0 && barycentricCoordinates.y >= 0 && barycentricCoordinates.z >= 0;
}

bool passZBufferTest(const Vector2i &point, const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &barycentricCoordinates) {
	float zValue = v0.z *barycentricCoordinates.x + v1.z * barycentricCoordinates.y + v2.z * barycentricCoordinates.z;

	if (zBuffer[point.y][point.x] < zValue) {
		zBuffer[point.y][point.x] = zValue;
		return true;
	} else {
		return false;
	}
}

Vector2i interpolateTextureCoordinates(const Vector3f &barycentric, const Vector3f &uv0, const Vector3f &uv1, const Vector3f &uv2) {
	Vector2i uvInterpolated;
	uvInterpolated.x = uv0.x *barycentric.x + uv1.x * barycentric.y + uv2.x * barycentric.z;
	uvInterpolated.y = uv0.y *barycentric.x + uv1.y * barycentric.y + uv2.y * barycentric.z;
	return uvInterpolated;
}

float applyGouraudInterpolation(const Vector3f &barycentric, float intensityN0, float intensityN1, float intensityN2) {
	return 1.0f - (intensityN0 *barycentric.x + intensityN1 * barycentric.y + intensityN2 * barycentric.z);
}

void applyLightIntensityToColour(float intensity, RGBA &colour) {
	colour.red = colour.red * intensity;
	colour.green = colour.green * intensity;
	colour.blue = colour.blue * intensity;
}

void drawBoundingBox(const BoundingBox &box, const RGBA &colour) {
	drawLine(box.min.x, box.min.y, box.max.x, box.min.y, colour);
	drawLine(box.min.x, box.max.y, box.max.x, box.max.y, colour);
	drawLine(box.min.x, box.min.y, box.min.x, box.max.y, colour);
	drawLine(box.max.x, box.min.y, box.max.x, box.max.y, colour);
}
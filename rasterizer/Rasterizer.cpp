#include "Rasterizer.h"
#include <algorithm>

Rasterizer::~Rasterizer() {
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void Rasterizer::createWindow() {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		exit(-1);
	}

	window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Rasterizer::clearBuffers() {
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			frameBuffer[j][i] = BLACK;
			zBuffer[j][i] = -std::numeric_limits<float>::max();
		}
	}
}

void Rasterizer::createViewportMatrix() {
	int x = SCREEN_WIDTH / 8;
	int y = SCREEN_HEIGHT / 8;
	int w = SCREEN_WIDTH * 3 / 4;
	int h = SCREEN_HEIGHT * 3 / 4;

	viewport = Matrix4f::identity();
	viewport[0][3] = x + w / 2.f;
	viewport[1][3] = y + h / 2.f;
	viewport[2][3] = 255 / 2.f;

	viewport[0][0] = w / 2.f;
	viewport[1][1] = h / 2.f;
	viewport[2][2] = 255 / 2.f;
}

void Rasterizer::createProjectionMatrix() {
	projection = Matrix4f::identity();
	projection[3][2] = -1.f / (camera.eye - camera.center).magnitude();
}

void Rasterizer::draw() {

	// Create the transform matrix 
	Matrix4f model = Matrix4f::identity();
	Matrix4f view = camera.lookat();
	projection[3][2] = -1.f / (camera.eye - camera.center).magnitude();
	Matrix4f transformationMatrix = viewport * projection * view * model;

	// draw faces of the mesh
	for (int i = 0; i < mesh->getFacesCount(); i++) {
		const FaceVector& face = mesh->getFace(i);
		Vector3f screenCoordinates[3];
		Vector3f texureCoordinates[3];
		Vector3f normals[3];
		Vector3f vertexCoordinates[3];
		for (int j = 0; j < 3; j++) {
			// call vertex shader
			vertexCoordinates[j] = mesh->getVertex(face[j].x);
			texureCoordinates[j] = mesh->getTextureCoordinate(face[j].y);
			normals[j] = mesh->getNormal(face[j].z);
			normals[j].normalize();
			screenCoordinates[j] = createFromHomogeneousMatrix(transformationMatrix*Matrix4f::fromVector(vertexCoordinates[j]));
		}
		drawTriangle(screenCoordinates, texureCoordinates, normals, *mesh);
	}

	SDL_UpdateTexture(texture, nullptr, frameBuffer, SCREEN_WIDTH * sizeof(byte) * 4);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}


bool Rasterizer::passZBufferTest(const Vector2i &point, const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &barycentricCoordinates) {
	float zValue = v0.z *barycentricCoordinates.x + v1.z * barycentricCoordinates.y + v2.z * barycentricCoordinates.z;

	if (zBuffer[point.y][point.x] < zValue) {
		zBuffer[point.y][point.x] = zValue;
		return true;
	} else {
		return false;
	}
}

void Rasterizer::drawBoundingBox(const BoundingBox &box, const RGBA &colour) {
	drawLine(box.min.x, box.min.y, box.max.x, box.min.y, colour);
	drawLine(box.min.x, box.max.y, box.max.x, box.max.y, colour);
	drawLine(box.min.x, box.min.y, box.min.x, box.max.y, colour);
	drawLine(box.max.x, box.min.y, box.max.x, box.max.y, colour);
}

Vector3f Rasterizer::createFromHomogeneousMatrix(const MatrixVectorf &m) {
	return Vector3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}


void Rasterizer::plotPixel(int x, int y, RGBA colour) {
	assert(x < SCREEN_WIDTH && y < SCREEN_HEIGHT);
	frameBuffer[SCREEN_HEIGHT - y][x] = colour;
}

void Rasterizer::drawLine(int x0, int y0, int x1, int y1, RGBA colour) {
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

void Rasterizer::drawTriangle(Vector3f vertices[3], Vector3f uvs[3], Vector3f normals[3], Mesh &mesh) {
	// check if triangle is degenerate to discard it
	if (isDegenerate(vertices[0], vertices[1], vertices[2])) {
		return;
	}

	// pre calculculate light intensity at normals
	float intensities[3];
	for (int i = 0; i < 3; i++) {
		intensities[i] = normals[i].dot(light);
	}

	// draw triangle
	BoundingBox box = calculateBoundingBoxOfTriangle(vertices[0], vertices[1], vertices[2]);
	for (int x = box.min.x; x <= box.max.x; x++) {
		for (int y = box.min.y; y <= box.max.y; y++) {
			Vector2i point = { x, y };
			Vector3f barycentric = calculateBarycentricCoordinates(point, vertices[0], vertices[1], vertices[2]);
			if (isPointInsideTriangle(barycentric) && passZBufferTest(point, vertices[0], vertices[1], vertices[2], barycentric)) {
				// Call fragment shader
				Vector2i uv = interpolateTextureCoordinates(barycentric, uvs[0], uvs[1], uvs[2]);
				RGBA colour = mesh.getTextureColor(uv);
				float lightIntensity = -applyGouraudInterpolation(barycentric, intensities[0], intensities[1], intensities[2]);
				colour.applyLightIntensity(std::max(0.0f, lightIntensity));
				plotPixel(x, y, colour);
			}
		}
	}
}

bool Rasterizer::isDegenerate(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2) {
	return v0.y == v1.y && v0.y == v2.y;
}

BoundingBox Rasterizer::calculateBoundingBoxOfTriangle(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2) {
	BoundingBox box;
	box.min = Vector2i(std::min({ v0.x, v1.x, v2.x, static_cast<float>(SCREEN_WIDTH - 1) }),
		std::min({ v0.y, v1.y, v2.y, static_cast<float>(SCREEN_HEIGHT - 1) }));

	box.max = Vector2i(std::max({ v0.x, v1.x, v2.x }),
		std::max({ v0.y, v1.y, v2.y }));

	box.max.x = std::min(box.max.x, SCREEN_WIDTH - 1);
	box.max.y = std::min(box.max.y, SCREEN_HEIGHT - 1);

	return box;
}

Vector3f Rasterizer::calculateBarycentricCoordinates(const Vector2i &point, const Vector3f &v0, const Vector3f &v1, const Vector3f &v2) {
	Vector3f a(v2.x - v0.x, v1.x - v0.x, v0.x - point.x);
	Vector3f b(v2.y - v0.y, v1.y - v0.y, v0.y - point.y);
	Vector3f u = a.cross(b);
	if (u.z == 0) {
		return Vector3f(-1, 1, 1);
	}
	return Vector3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

bool Rasterizer::isPointInsideTriangle(const Vector3f &barycentricCoordinates) {
	return barycentricCoordinates.x >= 0 && barycentricCoordinates.y >= 0 && barycentricCoordinates.z >= 0;
}


Vector2i Rasterizer::interpolateTextureCoordinates(const Vector3f &barycentric, const Vector3f &uv0, const Vector3f &uv1, const Vector3f &uv2) {
	Vector2i uvInterpolated;
	uvInterpolated.x = uv0.x *barycentric.x + uv1.x * barycentric.y + uv2.x * barycentric.z;
	uvInterpolated.y = uv0.y *barycentric.x + uv1.y * barycentric.y + uv2.y * barycentric.z;
	return uvInterpolated;
}

float Rasterizer::applyGouraudInterpolation(const Vector3f &barycentric, float intensityN0, float intensityN1, float intensityN2) {
	return intensityN0 *barycentric.x + intensityN1 * barycentric.y + intensityN2 * barycentric.z;
}
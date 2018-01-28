#include "Rasterizer.h"
#include <algorithm>

#include "../shaders/FaceIlluminationShader.h"
#include "../shaders/GouraudShader.h"
#include "../shaders/ClampIlluminationShader.h"
#include "../shaders/ZBufferShader.h"
#include "../shaders/PhongShader.h"
#include "../shaders/TangentNormalShader.h"

Rasterizer::Rasterizer(Mesh *mesh, Camera *camera) : mesh(mesh), camera(camera) {
	frameBuffer = new RGBA[SCREEN_WIDTH * SCREEN_HEIGHT];
	zBuffer = new float[SCREEN_WIDTH * SCREEN_HEIGHT];
	clearBuffers();
}

Rasterizer::~Rasterizer() {
	delete[] frameBuffer;
	delete[] zBuffer;

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
	memset(frameBuffer, 0x00, sizeof(RGBA) * SCREEN_HEIGHT * SCREEN_WIDTH);

	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
		zBuffer[i] = -std::numeric_limits<float>::max();
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
	projection[3][2] = -1.f / (camera->eye - camera->center).magnitude();
}

void Rasterizer::setUniformsInShader() {
	assert(shader != nullptr);

	switch (shader->getType()) {
		case ShaderType::FACE_ILLUMINATION:
		{
			FaceIlluminationShader * tmp = dynamic_cast<FaceIlluminationShader*>(shader.get());
			tmp->mesh = mesh;
			tmp->lightDirection = light;
			tmp->transform = transform;
		}
		break;

		case ShaderType::GOURAUD:
		{
			GouraudShader * tmp = dynamic_cast<GouraudShader*>(shader.get());
			tmp->mesh = mesh;
			tmp->lightDirection = light;
			tmp->transform = transform;
		}
		break;

		case ShaderType::CLAMP_ILUMINATION:
		{
			ClampIlluminationShader * tmp = dynamic_cast<ClampIlluminationShader*>(shader.get());
			tmp->mesh = mesh;
			tmp->lightDirection = light;
			tmp->transform = transform;
		}
		break;
		
		case ShaderType::ZBUFFER:
		{
			ZBufferShader * tmp = dynamic_cast<ZBufferShader*>(shader.get());
			tmp->mesh = mesh;
			tmp->zBuffer = zBuffer;
			tmp->depth = 320;
			tmp->screenWidth = SCREEN_WIDTH;
			tmp->transform = transform;
		}
		break;

		case ShaderType::PHONG:
		{
			PhongShader * tmp = dynamic_cast<PhongShader*>(shader.get());
			tmp->mesh = mesh;
			tmp->transform = transform;
			tmp->MWP = projection * view * mesh->getModelMatrix();
			tmp->lightDirection = MatrixVectorf::vectorFromHomogeneousMatrix(tmp->MWP * Matrix4f::homogeneousMatrixfromVector(light)).normalize();
			tmp->MWPInversedTransposed = (projection * view * mesh->getModelMatrix()).invertTranspose();
		}
		break;

		case ShaderType::TANGENT_NORMAL:
		{
			TangentNormalShader * tmp = dynamic_cast<TangentNormalShader*>(shader.get());
			tmp->mesh = mesh;
			tmp->transform = transform;
		}
		break;

		default:
			assert("uniforms not set");
		break;
	}
}

void Rasterizer::draw() {
	assert(shader != nullptr);

	// Create the transform matrix 
	model = mesh->getModelMatrix();
	view = camera->lookat();
	projection[3][2] = -1.f / (camera->eye - camera->center).magnitude();
	transform = viewport * projection * view * model;

	setUniformsInShader();

	// draw faces of the mesh
	for (int i = 0; i < mesh->getFacesCount(); i++) {
		const FaceVector& face = mesh->getFace(i);
		Vector3f screenCoordinates[3];
		for (int j = 0; j < 3; j++) {
			screenCoordinates[j] = shader->vertex(i, j);
		}

		shader->geometry(i, screenCoordinates);

		// back face culling
		Vector3f faceNormal = (screenCoordinates[1] - screenCoordinates[0]) ^ (screenCoordinates[2] - screenCoordinates[0]);
		faceNormal.normalize();
		if (faceNormal.dot(light) > 0.0f) {
			drawTriangle(screenCoordinates);
		}
	}

	SDL_UpdateTexture(texture, nullptr, frameBuffer, SCREEN_WIDTH * sizeof(byte) * 4);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}


bool Rasterizer::passZBufferTest(const Vector2i &point, const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &barycentricCoordinates) {
	float zValue = v0.z *barycentricCoordinates.x + v1.z * barycentricCoordinates.y + v2.z * barycentricCoordinates.z;

	int index = point.x + point.y * SCREEN_WIDTH;
	if (zBuffer[index] < zValue) {
		zBuffer[index] = zValue;
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

void Rasterizer::plotPixel(int x, int y, RGBA colour) {
	assert(x < SCREEN_WIDTH && y < SCREEN_HEIGHT);

	int index = x + ((SCREEN_HEIGHT - y) * SCREEN_WIDTH);
	frameBuffer[index] = colour;
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

void Rasterizer::drawTriangle(Vector3f vertices[3]) {
	// check if triangle is degenerate to discard it
	if (isDegenerate(vertices[0], vertices[1], vertices[2])) {
		return;
	}

	// draw triangle
	BoundingBox box = calculateBoundingBoxOfTriangle(vertices[0], vertices[1], vertices[2]);
	for (int x = box.min.x; x <= box.max.x; x++) {
		for (int y = box.min.y; y <= box.max.y; y++) {
			Vector2i point = { x, y };
			Vector3f barycentric = calculateBarycentricCoordinates(point, vertices[0], vertices[1], vertices[2]);
			if (isPointInsideTriangle(barycentric) && passZBufferTest(point, vertices[0], vertices[1], vertices[2], barycentric)) {
				// Call fragment shader
				shader->FRAGMENT_COORDINATES = point;
				RGBA colour = shader->fragment(barycentric);
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
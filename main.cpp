#include <stdlib.h>
#include <algorithm>
#include <assert.h>
#include <chrono>
#define SDL_MAIN_HANDLED

#include "types/Types.h"
#include "rasterizer/Mesh.h"
#include "rasterizer/Rasterizer.h"
#include "rasterizer/Camera.h"
#include "shaders/FaceIlluminationShader.h"
#include "shaders/GouraudShader.h"
#include "shaders/ClampIlluminationShader.h"

int main(int argc, char** argv) {

	// Load mesh and its textures
	Mesh mesh;
	mesh.loadObjFromFile("head.obj");
	mesh.loadDiffuseTexture("head_diffuse.png");

	// Create the camera
	Camera camera;
	camera.eye = Vector3f{ 0.0f, 0.0f, 3.0f };
	camera.center = Vector3f{ 0.0f, 0.0f, 0.0f };
	camera.up = Vector3f{ 0.0f, 1.0f, 0.0f };

	// Create the rasterizer and setup the transform matrices
	Rasterizer rasterizer(&mesh, &camera);
	rasterizer.createWindow();
	rasterizer.createProjectionMatrix();
	rasterizer.createViewportMatrix();

	// Create and set the light position in the rasterizer
	Vector3f light = Vector3f(0.0f, 0.0f, 1.0f).normalize();
	rasterizer.setLightPosition(light);

	// load shader
	std::unique_ptr<Shader> shader = std::unique_ptr<GouraudShader>(new GouraudShader());
	rasterizer.loadShader(shader);

	// init offset for camera movement
	float cameraOffset = 0.1;

	// timers for FPS counter
	std::chrono::time_point<std::chrono::high_resolution_clock> current, previous;
	std::chrono::duration<float, std::milli> elapsed(0);
	previous = std::chrono::high_resolution_clock::now();

	int frames = 0;

	SDL_Event event;
	bool quit = false;
	while (!quit) {
		// handle keyboard events
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}

			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_F1:
					{
						std::unique_ptr<Shader> shader = std::unique_ptr<GouraudShader>(new GouraudShader());
						rasterizer.loadShader(shader);
					}
					break;
					case SDLK_F2:
					{
						std::unique_ptr<Shader> shader = std::unique_ptr<FaceIlluminationShader>(new FaceIlluminationShader());
						rasterizer.loadShader(shader);
					}
					break;
					case SDLK_F3:
					{
						std::unique_ptr<Shader> shader = std::unique_ptr<ClampIlluminationShader>(new ClampIlluminationShader());
						rasterizer.loadShader(shader);
					}
					break;

					case SDLK_ESCAPE:
					{
						quit = true;
					}
					break;
				}
			}
		}

		rasterizer.draw();

		// move camera around
		camera.eye.x += cameraOffset;
		if (camera.eye.x >= 10.0f) {
			camera.eye.x -= cameraOffset;
			cameraOffset = -0.1f;
		} else if (camera.eye.x < -10.0f) {
			cameraOffset = 0.1f;
		}
		rasterizer.setCamera(&camera);
		rasterizer.clearBuffers();

		// FPS counter
		current = std::chrono::high_resolution_clock::now();
		elapsed += std::chrono::duration_cast<std::chrono::duration<float, std::milli>> (current - previous);
		previous = current;
		frames++;

		if (elapsed.count() >= 1000.0f) {
			rasterizer.setFpsCount(frames);
			elapsed = std::chrono::milliseconds::zero();
			frames = 0;
		}
	}

	return 0;
}
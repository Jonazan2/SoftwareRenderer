#include <stdlib.h>
#include <algorithm>
#include <assert.h>

#define SDL_MAIN_HANDLED

#include "types/Types.h"
#include "rasterizer/Mesh.h"
#include "rasterizer/Rasterizer.h"
#include "rasterizer/Camera.h"

int main(int argc, char** argv) {

	// Load mesh and its textures
	Mesh mesh;
	mesh.loadObjFromFile("head.obj");
	mesh.loadTexture("head_diffuse.png");

	// Create the camera
	Camera camera;
	camera.eye = Vector3f{ 2.0f, 1.0f, 3.0f };
	camera.center = Vector3f{ 0.0f, 0.0f, 0.0f };
	camera.up = Vector3f{ 0.0f, 1.0f, 0.0f };

	// Create the rasterizer and setup the transform matrices
	Rasterizer *rasterizer = new Rasterizer(&mesh, camera);
	rasterizer->createWindow();
	rasterizer->createProjectionMatrix();
	rasterizer->createViewportMatrix();
	rasterizer->setLightPosition(Vector3f(0.0f, -1.0f, 1.0f).normalize());

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

		rasterizer->draw();
	}

	delete rasterizer;
	return 0;
}
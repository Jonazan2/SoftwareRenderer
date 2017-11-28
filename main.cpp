#include <stdlib.h>
#include <algorithm>
#include <assert.h>

#define SDL_MAIN_HANDLED 
#include <SDL.h>

static const int SCREEN_WIDTH = 1024;
static const int SCREEN_HEIGHT = 768;

using byte = unsigned char;

struct RGBA {
	byte red;
	byte green;
	byte blue;
	byte alpha;

	bool isEqual(RGBA other) {
		return red == other.red && green == other.green
			&& blue == other.blue && alpha == other.alpha;
	}
};


static const RGBA BLACK = { 0x00, 0x00, 0x00, 0xFF };
static const RGBA PINK = { 0xFF, 0xAA, 0xBB, 0xFF };

RGBA frameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

void plotPixel(int x, int y, RGBA colour);
void drawLine(int x0, int y0, int x1, int y1, RGBA colour);

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		return -1;
	}

	SDL_Window *window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);


	// init frame buffer to PINK
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			frameBuffer[j][i] = PINK;
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

		drawLine(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
		drawLine(0, SCREEN_HEIGHT/ 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, BLACK);
		drawLine(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, BLACK);
		drawLine(0, SCREEN_HEIGHT - 1, SCREEN_WIDTH - 1, 0, BLACK);

		SDL_UpdateTexture(texture, NULL, frameBuffer, SCREEN_WIDTH * sizeof(byte) * 4);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	return 0;
}

void plotPixel(int x, int y, RGBA colour) {
	assert(x < SCREEN_WIDTH && y < SCREEN_HEIGHT);
	frameBuffer[y][x] = colour;
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
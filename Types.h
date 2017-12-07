#pragma once

#include "Vector2.h"

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
static const RGBA WHITE = { 0xFF, 0xFF, 0xFF, 0xFF };
static const RGBA BLUE = { 0x00, 0x00, 0xFF, 0xFF };
static const RGBA RED = { 0xFF, 0x00, 0x00, 0xFF };

struct BoundingBox {
	Vector2i min;
	Vector2i max;
};
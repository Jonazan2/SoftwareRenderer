#pragma once

#include <cassert>
#include <cmath>

template <typename T>
class Vector2 {
public:
	T x, y;

	Vector2() : x(0), y(0) {}
	Vector2(T x, T y) : x(x), y(y) {}
	Vector2(const Vector2& other) : x(other.x), y(other.y) {}

	T& operator[](size_t index) {
		assert(index < 2);
		switch (index) {
			case 0: return x;
			case 1: return y;
		}
	}

	Vector2 operator /(T scalar) const {
		assert(scalar != 0);
		T inversedScalar = 1.0f / scalar;
		return (*this) * inversedScalar;
	}

	Vector2& operator/=(T scalar) {
		assert(scalar != 0);
		T inversedScalar = 1.0f / scalar;
		x *= inversedScalar;
		y *= inversedScalar;
		return (*this);
	}

	Vector2 operator*(T scalar) const {
		return Vector2(x * scalar, y * scalar);
	}

	Vector2& operator*=(T scalar) {
		x *= scalar;
		y *= scalar;
		return (*this);
	}

	Vector2 operator-(const Vector2 &other) const {
		return { x - other.x, y - other.y };
	}

	Vector2& operator-=(const Vector2 &other) {
		x - other.x;
		y - other.y;
		return *this;
	}

	Vector2 operator+(const Vector2 &other) const {
		return (x + other.x, y + other.y);
	}

	Vector2& operator+=(const Vector2 &other) {
		x + other.x;
		y + other.y;
		return *this;
	}

	T magnitude() const {
		return sqrt((pow(x,2) + pow(y, 2)));
	}

	Vector2& normalize() {
		*this = *this / magnitude()
		return *this;
	}

	Vector2 getNormalizeVector() const {
		return *this / magnitude()
	}

	T dot(const Vector2 &other) const {
		return (x * other.x) + (y * other.y);
	}
};

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
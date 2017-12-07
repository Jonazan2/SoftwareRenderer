#pragma once

#include <assert.h>
#include <math.h>

template <typename T>
class Vector3 {
public:
	T x, y, z;

	Vector3() : x(0), y(0), z(0) {}
	Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
	Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}

	T& operator[](size_t index) {
		assert(index < 3);
		switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
		}
	}

	Vector3 operator /(T scalar) const {
		assert(scalar != 0);
		T inversedScalar = 1.0f / scalar;
		return (*this) * inversedScalar;
	}

	Vector3& operator/=(T scalar) {
		assert(scalar != 0);
		T inversedScalar = 1.0f / scalar;
		x *= inversedScalar;
		y *= inversedScalar;
		z *= inversedScalar;
		return (*this);
	}

	Vector3 operator*(T scalar) const {
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	Vector3& operator*=(T scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return (*this);
	}

	Vector3 operator-(const Vector3 &other) const {
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	Vector3& operator-=(const Vector3 &other) {
		x - other.x;
		y - other.y;
		z - other.z;
		return *this;
	}

	Vector3 operator+(const Vector3 &other) const {
		return (x + other.x, y + other.y, z + other.z);
	}

	Vector3& operator+=(const Vector3 &other) {
		x + other.x;
		y + other.y;
		z + other.z;
		return *this;
	}

	T magnitude() const {
		return sqrt((pow(x,2) + pow(y, 2) + pow(z, 2)));
	}

	Vector3& normalize() {
		*this = *this / magnitude();
		return *this;
	}

	Vector3 getNormalizeVector() const {
		return *this / magnitude()
	}

	Vector3 cross(const Vector3 &other) const {
		return Vector3(y * other.z - z*other.y,
			z*other.x - x*other.z,
			x*other.y - y*other.x);
	}

	Vector3& operator^=(const Vector3 &other) {
		x = y * other.z - z*other.y;
		y = z*other.x - x*other.z;
		z = x*other.y - y*other.x;
		return *(this);
	}

	Vector3 operator^(const Vector3 &other) {
		return Vector3(y * other.z - z*other.y,
			z*other.x - x*other.z,
			x*other.y - y*other.x);
	}

	T dot(const Vector3 &other) const {
		return (x * other.x) + (y * other.y) + (z * other.z);
	}
};

using Vector3f = Vector3<float>;
using Vector3i = Vector3<int>;
#pragma once

#include <assert.h>
#include <math.h>

template <typename T>
class Vector3 {
public:
	T x, y, z;

	Vector3(T x, T y, T z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3(Vector3 &other) {
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
	}

	Vector3 operator /(T scalar) const {
		T inversedScalar = 1.0f / scalar;
		return (*this) * inversedScalar;
	}

	Vector3& operator/=(T scalar) {
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
		return (x - other.x, y - other.y, z - other.z);
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
		*this = *this / magnitude()
		return *this;
	}

	Vector3 getNormalizeVector() const {
		return *this / magnitude()
	}
};


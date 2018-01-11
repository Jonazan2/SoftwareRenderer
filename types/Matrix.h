#pragma once

#include <vector>
#include <iostream>
#include "Vector3.h"

template <typename T, size_t rowDimension, size_t colDimension>
class Matrix {
public:

	static Matrix<T, rowDimension, rowDimension> identity() {
		Matrix<T, rowDimension, rowDimension> identityMatrix;
		for (int i = 0; i < rowDimension; i++) {
			identityMatrix[i][i] = static_cast<T>(1);
		}
		return identityMatrix;
	}

	static Matrix<T, 4, 1> fromVector(const Vector3f &vector) {
		Matrix<T, 4, 1> matrix;
		matrix[0][0] = vector.x;
		matrix[1][0] = vector.y;
		matrix[2][0] = vector.z;
		matrix[3][0] = static_cast<T>(1);
		return matrix;
	}

	static Vector3f createFromHomogeneousMatrix(const Matrix<float, 4, 1> &m) {
		return Vector3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
	}

	Matrix() {
		memset(matrix, static_cast<T>(0), sizeof(T) * rowDimension * colDimension);
	}

	Matrix(std::initializer_list<std::initializer_list<T>> set) {
		memset(matrix, static_cast<T>(0), sizeof(T) * rowDimension * colDimension);

		std::initializer_list<std::initializer_list<T>>::const_iterator it = set.begin();
		for (int i = 0; it != set.end(); i++, ++it) {
			std::initializer_list<T >::const_iterator rowIterator = it->begin();
			for (int j = 0; rowIterator != it->end(); j++, ++rowIterator) {
				matrix[i][j] = *rowIterator;
			}
		}
	}

	Matrix<T, colDimension, rowDimension> getTransposed() {
		Matrix<T, colDimension, rowDimension> transposed;
		for (int i = 0; i < rowDimension; i++) {
			for (int j = 0; j < colDimension; j++) {
				transposed[j][i] = matrix[i][j];
			}
		}
		return transposed;
	}

	void print() {
		for (int i = 0; i < rowDimension; i++) {
			for (int j = 0; j < colDimension; j++) {
				std::cout << matrix[i][j] << " ";
			}
			std::cout << "\n";
		}
	}

	Matrix<T, rowDimension, colDimension> operator+(const Matrix<T, rowDimension, colDimension>& other) const {
		Matrix<T, rowDimension, colDimension> result;
		for (int i = 0; i < rowDimension; i++) {
			for (int j = 0; j < colDimension; j++) {
				result[i][j] = matrix[i][j] + other[i][j];
			}
		}
		return result;
	}

	Matrix<T, rowDimension, colDimension> operator-(const Matrix<T, rowDimension, colDimension>& other) const {
		Matrix<T, rowDimension, colDimension> result;
		for (int i = 0; i < rowDimension; i++) {
			for (int j = 0; j < colDimension; j++) {
				result[i][j] = matrix[i][j] - other[i][j];
			}
		}
		return result;
	}

	T* operator[](size_t index) {
		assert(index < rowDimension);
		return matrix[index];
	}

	const T* const operator[](size_t index) const {
		assert(index < rowDimension);
		return matrix[index];
	}

	constexpr size_t getRowDimension() { return rowDimension; }
	constexpr size_t getColumnDimension() { return colDimension; }

private:
	T matrix[rowDimension][colDimension];
};

template <typename T, size_t rowsLeft, size_t colLeft, size_t colRight>
Matrix<T, rowsLeft, colRight> operator*(const Matrix<T, rowsLeft, colLeft> &lhs, const Matrix<T, colLeft, colRight> &rhs) {
	Matrix<T, rowsLeft, colRight> result;
	for (int r1 = 0; r1 < rowsLeft; r1++) {
		for (int c2 = 0; c2 < colRight; c2++) {
			T ac = 0;
			for (int c1 = 0; c1 < colLeft; c1++) {
				ac += lhs[r1][c1] * rhs[c1][c2];
			}
			result[r1][c2] = ac;
		}
	}
	return result;
}

using Matrix4f = Matrix<float, 4, 4>;
using MatrixVectorf = Matrix<float, 4, 1>;
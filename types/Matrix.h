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

	static Matrix<T, 3, 1> createFromVector(const Vector3f &vector) {
		Matrix<T, 3, 1> matrix;
		matrix[0][0] = vector.x;
		matrix[1][0] = vector.y;
		matrix[2][0] = vector.z;
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
		for (int i = 0; i < rowDimension; ++i) {
			for (int j = 0; j < colDimension; j++) {
				transposed[j][i] = matrix[i][j];
			}
		}
		return transposed;
	}

	T determinant() const {
		return dt<T, rowDimension>::determinant(*this);
	}

	Matrix<T, rowDimension - 1, colDimension - 1> get_minor(size_t row, size_t col) const {
		Matrix<T, rowDimension - 1, colDimension - 1> ret;
		for (int i = rowDimension - 2; i >= 0; --i) {
			for (int j = colDimension - 2; j >= 0; --j) {
				ret[i][j] = matrix[i < row ? i : i + 1][j < col ? j : j + 1];
			}
		}
		return ret;
	}

	T cofactor(size_t row, size_t col) const {
		return get_minor(row, col).determinant()*((row + col) % 2 ? -1 : 1);
	}

	Matrix<T, rowDimension, colDimension> adjugate() const {
		Matrix<T, rowDimension, colDimension> ret;
		for (int i = rowDimension - 1; i >= 0; --i) {
			for (int j = colDimension - 1; j >= 0; --j) {
				ret[i][j] = cofactor(i, j);
			}
		}
		return ret;
	}

	Matrix<T, rowDimension, colDimension> invertTranspose() {
		Matrix<T, rowDimension, colDimension> ret = adjugate();
		T ac = 0;
		for (int i = 0; i < rowDimension; ++i) {
			ac += ret[0][i] * matrix[0][i];
		}
		return ret;
	}

	Matrix<T, rowDimension, colDimension> invert() {
		return invertTranspose().getTransposed();
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

	bool operator==(const Matrix<T, rowDimension, colDimension> &rhs) {
		for (int i = 0; i < rowDimension; i++) {
			for (int j = 0; j < colDimension; j++) {
				if (matrix[i][j] != rhs[i][j]) {
					return false;
				}
			}
		}
		return true;
	}

	constexpr size_t getRowDimension() { return rowDimension; }
	constexpr size_t getColumnDimension() { return colDimension; }

private:
	T matrix[rowDimension][colDimension];
};

template<typename T, size_t dimensions> struct dt {
	static T determinant(const Matrix<T, dimensions, dimensions>& src) {
		T ret = 0;
		for (int i = dimensions - 1; i >= 0; i--) {
			ret += src[0][i] * src.cofactor(0, i);
		}
		return ret;
	}
};

template<typename T> struct dt<T, 1> {
	static T determinant(const Matrix<T, 1, 1>& src) {
		return src[0][0];
	}
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

using Matrix3f = Matrix<float, 3, 3>;
using Matrix4f = Matrix<float, 4, 4>;
using MatrixVectorf = Matrix<float, 4, 1>;
#include <vector>
#include "Vector3.h"
#include <iostream>

template <typename T>
class Matrix {
public:
	static Matrix identity(size_t dimensions) {
		Matrix m(dimensions, dimensions);
		for ( int i = 0; i < dimensions; i++ ) {
			m[i][i] = static_cast<T>(1);
		}
		return m;
	}

	static Matrix createHomogeneousMatrixFromVector3(const Vector3<T>& vector) {
		Matrix m(4, 1);
		m[0][0] = vector.x;
		m[0][1] = vector.y;
		m[0][2] = vector.z;
		m[0][3] = static_cast<T>(0);
		return m;
	}

	Matrix(size_t rows, size_t columns) : rows(rows), columns(columns), matrix(rows, std::vector<T>(columns, static_cast<T>(0))) {}
	
	Matrix(Vector3<T> vector) : rows(3), columns(1), matrix(rows, std::vector<T>(columns, static_cast<T>(0))) {
		matrix[0][0] = vector.x;
		matrix[0][1] = vector.y;
		matrix[0][2] = vector.z;
	}


	Matrix getTransposed() const {
		Matrix m(this);
		m.transpose;
		return m;
	}

	void transpose() {
		std::vector<std::vector<T>> transposed(columns, std::vector<T>(rows, static_cast<T>(0)));

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < columns; j++) {
				transposed[j][i] = matrix[i][j];
			}
		}

		const T aux = rows;
		this->rows = columns;
		this->columns = aux;
		matrix = transposed;
	}

	void print() const {

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < columns; j++) {
				std::cout << matrix[i][j] << " ";
			}
			std::cout << "\n";
		}
	}

	Matrix operator*(Matrix& other) {
		assert(columns == other.getRowsCount());

		Matrix result(rows, other.getColumnsCount());
		for ( int r1 = 0; r1 < rows; r1++ ) {
			for ( int c2 = 0; c2 < other.getColumnsCount(); c2++ ) {
				for ( int c1 = 0; c1 < columns; c1++ ) {
					result[r1][c2] += matrix[r1][c1] * other[c1][c2];
				}
			}
		}

		return result;
	}

	Matrix operator+(const Matrix& other) {
		Matrix result(other);

		for (int i = 0; i < other.getRowsCount(); i++) {
			for (int j = 0; j < other.getColumnsCount(); j++) {
				result[i][j] = matrix[i][j] + other[i][j];
			}
		}

		return result;
	}

	Matrix& operator+=(const Matrix& other) {
		for (int i = 0; i < other.getRowsCount(); i++) {
			for (int j = 0; j < other.getColumnsCount(); j++) {
				matrix[i][j] += other[i][j];
			}
		}

		return (*this);
	}

	std::vector<T>& operator[](size_t index) {
		assert(index < rows);
		return matrix[index];
	}

	size_t getRowsCount() const { return rows; }
	size_t getColumnsCount() const { return columns; }


private:
	size_t rows;
	size_t columns;

	std::vector<std::vector<T>> matrix;
};
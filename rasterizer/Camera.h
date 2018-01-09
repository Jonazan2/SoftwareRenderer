#pragma once

#include "../types/Vector3.h"
#include "../types/Matrix.h"

struct Camera {
	Vector3f eye;
	Vector3f center;
	Vector3f up;

	Matrix4f lookat() {
		Vector3f z = (eye - center).normalize();
		Vector3f x = (up^z).normalize();
		Vector3f y = (z^x).normalize();
		Matrix4f res = Matrix4f::identity();
		for (int i = 0; i<3; i++) {
			res[0][i] = x[i];
			res[1][i] = y[i];
			res[2][i] = z[i];
			res[i][3] = -center[i];
		}
		return res;
	}
};
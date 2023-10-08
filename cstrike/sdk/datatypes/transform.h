#pragma once

// used: matResult
#include "matrix.h"
// used: quaternion
#include "quaternion.h"

class alignas(16) CTransform
{
public:
	alignas(16) Vector_t vecPosition;
	alignas(16) Quaternion_t quatOrientation;

	void GetMatrix(Matrix3x4_t& matOut) const
	{
		matOut[0][0] = 1.0f - 2.0f * quatOrientation.y * quatOrientation.y - 2.0f * quatOrientation.z * quatOrientation.z;
		matOut[1][0] = 2.0f * quatOrientation.x * quatOrientation.y + 2.0f * quatOrientation.w * quatOrientation.z;
		matOut[2][0] = 2.0f * quatOrientation.x * quatOrientation.z - 2.0f * quatOrientation.w * quatOrientation.y;

		matOut[0][1] = 2.0f * quatOrientation.x * quatOrientation.y - 2.0f * quatOrientation.w * quatOrientation.z;
		matOut[1][1] = 1.0f - 2.0f * quatOrientation.x * quatOrientation.x - 2.0f * quatOrientation.z * quatOrientation.z;
		matOut[2][1] = 2.0f * quatOrientation.y * quatOrientation.z + 2.0f * quatOrientation.w * quatOrientation.x;

		matOut[0][2] = 2.0f * quatOrientation.x * quatOrientation.z + 2.0f * quatOrientation.w * quatOrientation.y;
		matOut[1][2] = 2.0f * quatOrientation.y * quatOrientation.z - 2.0f * quatOrientation.w * quatOrientation.x;
		matOut[2][2] = 1.0f - 2.0f * quatOrientation.x * quatOrientation.x - 2.0f * quatOrientation.y * quatOrientation.y;

		matOut[0][3] = vecPosition.x;
		matOut[1][3] = vecPosition.y;
		matOut[2][3] = vecPosition.z;
	}
};

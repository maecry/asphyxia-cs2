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

	const Matrix3x4_t& GetMatrix() const
	{
		return quatOrientation.ToMatrix(vecPosition);
	}
};

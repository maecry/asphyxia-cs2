#pragma once

// used: matResult
#include "matrix.h"
// used: quaternion
#include "quaternion.h"

class CTransform
{
public:
	VectorAligned_t vecPosition;
	QuaternionAligned_t quatOrientation;
};

static_assert(alignof(CTransform) == 16);

#include "Vector3.hpp"

/* Implementation for Vec3Dot */
float Vec3Dot(const Vector3* pV1, const Vector3* pV2)
{

	return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;
}

float clamp(float min, float max, float val)
{
	return (val > max) ? max : (val < min) ? min : val;
}
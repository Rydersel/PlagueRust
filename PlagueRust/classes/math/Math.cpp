/**
 * <3 skit#2567
 *
 * don't make this into a shitty p2c
 */

#include "Math.hpp"

bool InRange(float test, float val, float offset)
{
	if (val > test - offset && val < test + offset)
	{
		return true;
	}
	return false;
}
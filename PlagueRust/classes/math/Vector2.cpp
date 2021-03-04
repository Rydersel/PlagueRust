#include "Vector2.hpp"
#include <algorithm>
#include <cmath>

Vector2::Vector2()
{
	// Initialize x, y to 0, 0
	x = 0, y = 0;
}

/*
 * Implementation for Math::Vector2's constructor
 */
Vector2::Vector2(float init_x, float init_y)
{
	// Initialize x, y to user-defined values
	x = init_x, y = init_y;
}

/*
 * Implementation for Math::Vector2::Lerp (end, t)
 */
Vector2 Vector2::Lerp(Vector2 end, float t)
{
	// Clamp 't' between 0 and 1
	t = std::clamp(t, 0.0f, 1.0f);

	// Instantiate a new vector with lerped values
	return {
		x + (end.x - x) * t,
		y + (end.y - y) * t
	};
}

/*
 * Implementation for Math::Vector2::Lerp (t)
 */
Vector2 Vector2::Lerp(float t)
{
	// Clamp 't' between 0 and 1
	t = std::clamp(t, 0.0f, 1.0f);

	// Instantiate a new vector with lerped values
	return {
		x * t,
		y * t
	};
}

/*
 * Implementation for Math::Vector2::Floor
 */
Vector2 Vector2::Floor()
{
	return {
		std::floor(x),
		std::floor(y)
	};
}

/*
 * Implementation for Math::Vector2::Ceil
 */
Vector2 Vector2::Ceil()
{
	return {
		std::ceil(x),
		std::ceil(y)
	};
}

/*
 * Implementation for Math::Vector2::Length
 */
float Vector2::Length()
{
	// Calculation for euclidian length (magnitude)
	return sqrt(
		pow(x, 2) + pow(y, 2)
	);
}

/*
 * Implementation for Math::Vector2::Abs
 */
Vector2 Vector2::Abs()
{
	return {
		fabs(x),
		fabs(y)
	};
}
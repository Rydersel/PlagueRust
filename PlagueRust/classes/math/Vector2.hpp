#ifndef _VECTOR2_HPP
#define _VECTOR2_HPP

/* A vector2 class */
class Vector2
{
public:

	/*
	 * Used to initialize this class with no values
	 */
	Vector2();

	/*
	 * Used to initialize this class with given values
	 *
	 * Args:
	 *  x: the x-value for this vector2
	 *  y: the y-value for this vector2
	 *
	 * Returns:
	 *  A new vector2 containing your defined vlaues
	 */
	Vector2(float x, float y);

	/*
	 * Used to lerp between 0,0 and this vector's values
	 *
	 * Args:
	 *  t: A value between 0 and 1 representing the position in the lerp you want values for
	 *
	 * Returns:
	 *  The lerped values given t
	 */
	Vector2 Lerp(float t);

	/*
	 * Used to linearly interpolate between two vectors
	 *
	 * Args:
	 *  t: A value between 0 and 1 representing the position in the lerp you want values for
	 *  end: The vector you want to lerp between
	 *
	 * Returns:
	 *  The lerped values between this vector and 'end' vector given 't'
	 */
	Vector2 Lerp(Vector2 end, float t);

	/*
	 * Used to get the lower rounded natural numbers for this vector
	 *
	 * Returns:
	 *  A new vector containing natural numbers
	 */
	Vector2 Floor();

	/*
	 * Used to get the upper rounded natural numbers for this vector
	 *
	 * Returns:
	 *  A new vector containing natural numbers
	 */
	Vector2 Ceil();

	/*
	 * Convert a vector2 to it's absolute values
	 */
	Vector2 Abs();

	/*
	 * Used to get the euclidian length of a vector
	 */
	float Length();

	/*
	 * Overload Vector2 subtract operator
	 */
	Vector2 operator - (Vector2 other)
	{
		return {
			x - other.x,
			y - other.y
		};
	}

	/*
	 * Overload Vector2 addition operator
	 */
	Vector2 operator + (Vector2 other)
	{
		return {
			x + other.x,
			y + other.y
		};
	}

	/*
	 * Overload the Vector2 multiplication operator
	 */
	Vector2 operator * (Vector2 other)
	{
		return {
			x * other.x,
			y * other.y
		};
	}

	/*
	 * Overload for the Vector2 division operator
	 */
	Vector2 operator / (Vector2 other)
	{
		return {
			x / other.x,
			y / other.y
		};
	}

	/*
	 * Overload for the add assign operator
	 */
	void operator += (Vector2 other)
	{
		// Update this vector's values
		x += other.x;
		y += other.y;
	}

	/*
	 * Overload for the subtract assign operator
	 */
	void operator -= (Vector2 other)
	{
		// Update this vector's values
		x -= other.x;
		y -= other.y;
	}

	/*
	 * Overload for the multiply assign operator
	 */
	void operator *= (Vector2 other)
	{
		// Update this vector's values
		x *= other.x;
		y *= other.y;
	}

	/*
	 * Overload for the divide assign operator
	 */
	void operator /= (Vector2 other)
	{
		// Update this vector's values
		x /= other.x;
		y /= other.y;
	}

public: // Public fields for this class
	float x, y;
};

#endif // _VECTOR2_HPP
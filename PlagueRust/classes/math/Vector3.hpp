#ifndef _VECTOR3_HPP
#define _VECTOR3_HPP

/* Includes */
#include <cmath>

/* Clamp function */
float clamp(float min, float max, float val);

/* A vector3 class */
class Vector3
{
public:
	/* Default constructor for this class */
	Vector3() 
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
	}

	/* Constructor for this class */
	Vector3(float _x, float _y, float _z)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
	}

	/* Used to calculate the distance between two vector3s */
	float Distance(Vector3 b)
	{
		Vector3 calc(this->x - b.x, this->y - b.y, this->z - b.z);

		/* Calculate the distance */
		return sqrt(calc.x * calc.x + calc.y * calc.y + calc.z * calc.z);
	}

	/* Used to calculate the distance between two vector3s */
	float Distance2(Vector3 b)
	{
		Vector3 calc(this->x - b.x, this->y - b.y, this->z - b.z);

		/* Calculate the distance */
		return sqrtf(pow(calc.x, 2) + pow(calc.y, 2) + pow(calc.z, 2));
	}

	/* Used to get the magnitude of this vector */
	float Magnitude()
	{
		return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
	}

	/* Used to normalize this vector */
	Vector3 Normalized()
	{
		/* Get the magnitude of this vector */
		float num = this->Magnitude();

		/* Check if number is more than max. */
		if (num > 9.99999974737875E-06)
		{
			return { this->x / num, this->y / num, this->z / num };
		}

		/* Return an empty vector */
		return Vector3();
	}

	/* Used to calculate the dot product of this, and another vector */
	float Dot(Vector3 rhs)
	{
		return (this->x * rhs.x + this->y * rhs.y + this->z * rhs.z);
	}

	/* Used to calculate the normalized dot */
	float DotNormalized(Vector3 rhs)
	{
		/* Normalize both vectors */
		Vector3 lhs = this->Normalized();
		rhs = rhs.Normalized();

		return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
	}

	/* Returns the angle in degrees between this vec, and to */
	float Angle(Vector3 to)
	{
		return acos(clamp(-1.f, 1.f, this->DotNormalized(to))) * 57.29578f;
	}

public:

	/* Overload the + operator */
	Vector3 operator + (const Vector3& b)
	{
		return {
			this->x + b.x,
			this->y + b.y,
			this->z + b.z
		};
	}

	/* Overload the + operator */
	Vector3 operator + (float b)
	{
		return {
			this->x + b,
			this->y + b,
			this->z + b
		};
	}

	/* Overload the + operator */
	Vector3 operator - (const Vector3& b)
	{
		return {
			this->x - b.x,
			this->y - b.y,
			this->z - b.z
		};
	}

	/* Overload the - operator */
	Vector3 operator - (float b)
	{
		return {
			this->x - b,
			this->y - b,
			this->z - b
		};
	}

	/* Overload the + operator */
	Vector3 operator * (const Vector3& b)
	{
		return {
			this->x * b.x,
			this->y * b.y,
			this->z * b.z
		};
	}

	/* Overload the * perator */
	Vector3 operator * (float b)
	{
		return {
			this->x * b,
			this->y * b,
			this->z * b
		};
	}

	/* Overload the + operator */
	Vector3 operator / (const Vector3& b)
	{
		return {
			this->x / b.x,
			this->y / b.y,
			this->z / b.z
		};
	}

	/* Overload the / operator */
	Vector3 operator / (float b)
	{
		return {
			this->x / b,
			this->y / b,
			this->z / b
		};
	}

	/* Overload the + operator */
	void operator += (const Vector3& b)
	{

		this->x += b.x;
		this->y += b.y;
		this->z += b.z;
	}

	/* Overload the + operator */
	void operator += (float b)
	{
		this->x += b;
		this->y += b;
		this->z += b;
	}

	/* Overload the + operator */
	void operator -= (const Vector3& b)
	{
		this->x -= b.x;
		this->y -= b.y;
		this->z -= b.z;
	}

	/* Overload the + operator */
	void operator -= (float b)
	{

		this->x -= b;
		this->y -= b;
		this->z -= b;
		
	}

	/* Overload the + operator */
	void operator *= (const Vector3& b)
	{
		this->x *= b.x;
		this->y *= b.y;
		this->z *= b.z;
	}

	/* Overload the + operator */
	void operator *= (float b)
	{
		this->x *= b;
		this->y *= b;
		this->z *= b;
	}

	/* Overload the + operator */
	void operator /= (const Vector3& b)
	{
		this->x /= b.x;
		this->y /= b.y;
		this->z /= b.z;
	}

	/* Overload the + operator */
	void operator /= (float b)
	{
		this->x /= b;
		this->y /= b;
		this->z /= b;
	}

public:

	// Axial values
	float x;
	float y;
	float z;


};

/* Used to calculate the dot product of a vector3 */
float Vec3Dot(const Vector3* pV1, const Vector3* pV2);

#endif // _VECTOR3_HPP
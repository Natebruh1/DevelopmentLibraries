#pragma once
#include <cmath>

//We use a union to allow for access to the vector components as both individual floats and as an array. I.E v[0] is x, v[1] is y, and v[2] is z. or v.x, v.y, v.z
union vector3
{
	//Data
	struct { float x, y, z; };
	float v[3];

	//Member functions

	inline float length() const
	{
		return std::hypotf(std::hypotf(x,y),z);
	}
	float lengthSquared() const
	{
		return x * x + y * y + z * z;
	}
	//Returns the largest component
	float max()
	{
		float m = x;
		(m < y) && (m = y); //these are not conditional statements.
		(m < z) && (m = z); //these are just boolean expressions.
		return m;
		
	}
	//Returns the largest component index
	float maxIndex()
	{
		return (y > x) * ((y > x) + (z > y)) + ((x>y)*(z>x)*2);
	}

	vector3 normalize() const
	{
		float len = length();
		return (len > 0.0f) ? *this / len : vector3{ 0,0,0 };
	}

	float dist(const vector3& o) //Returns the distance to another vector3 object
	{
		return std::sqrtf((o.x - x) * (o.x - x) + (o.y - y) * (o.y - y) + (o.z - z) * (o.z - z));
	}
	float distSquared(const vector3& o)
	{
		return (o.x - x) * (o.x - x) + (o.y - y) * (o.y - y) + (o.z - z) * (o.z - z);
	}
	vector3 operator-(vector3 const& rhs) const
	{
		return { x - rhs.x,y - rhs.y,z - rhs.z };
	}
	vector3 operator+(vector3 const& rhs) const
	{
		return { x + rhs.x,y + rhs.y,z + rhs.z };
	}
	vector3 operator*(vector3 const& rhs) const
	{
		return { x * rhs.x,y * rhs.y,z * rhs.z };
	}

	vector3 operator*(float scalar) const
	{
		return { x * scalar, y * scalar, z * scalar };
	}
	vector3 operator/(float scalar) const
	{
		return { x / scalar, y / scalar, z / scalar };
	}
};
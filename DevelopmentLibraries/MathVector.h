#pragma once
#include <cmath>

union vector3
{
	struct { float x, y, z; };
	float v[3];
	float length()
	{
		return std::sqrtf(x * x + y * y + z * z);
	}
	float lengthSquared()
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


	float dist(vector3 o)
	{
		return std::sqrtf((o.x - x) * (o.x - x) + (o.y - y) * (o.y - y) + (o.z - z) * (o.z - z));
	}
	float distSquared(vector3 o)
	{
		return (o.x - x) * (o.x - x) + (o.y - y) * (o.y - y) + (o.z - z) * (o.z - z);
	}
	vector3 operator-(vector3 const& rhs)
	{
		return { x - rhs.x,y - rhs.y,z - rhs.z };
	}
	vector3 operator+(vector3 const& rhs)
	{
		return { x + rhs.x,y + rhs.y,z + rhs.z };
	}
	vector3 operator*(vector3 const& rhs)
	{
		return { x * rhs.x,y * rhs.y,z * rhs.z };
	}
};
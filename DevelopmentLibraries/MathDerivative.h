#pragma once
#include <math.h>
#include "MathVector.h"

/// <summary>
/// Forward Mode Automatic Differentiation
/// </summary>

struct Dual
{
	float real;
	float dual;
	Dual(float r, float d)
	{
		real = r;
		dual = d;
	}
	Dual operator+(Dual const& rhs)
	{
		float r = real + rhs.real;
		float d = dual + rhs.dual;
		return Dual(r, d);
	}

	Dual operator-(Dual const& rhs)
	{
		float r = real - rhs.real;
		float d = dual - rhs.dual;
		return Dual(r, d);
	}

	Dual operator*(Dual const& rhs)
	{
		float r = real * rhs.real;
		float d = real * rhs.dual + dual * rhs.real;
		return Dual(r, d);
	}

	Dual operator+(int const& rhs)
	{
		return Dual(real + rhs, dual + rhs);
	}
	Dual operator-(int const& rhs)
	{
		return Dual(real- rhs, dual - rhs);
	}

	
};






inline Dual operator*(int x, Dual const& rhs)
{
	bool neg = (int)signbit((double)x)>0?true : false;
	
	if (neg)
	{
		Dual total =Dual(0,0);
		for (int i = 0; i < abs(x); i++)
		{
			total = total-rhs;
		}
		return total;
	}
	else
	{
		Dual total = Dual(0,0);
		for (int i = 0; i < x; i++)
		{
			total = total + rhs;
		}
		return total;
	}
}

inline Dual operator-(float x, Dual const& rhs)
{
	return Dual(rhs.real - x, rhs.dual - x);
}
inline Dual operator+(float x, Dual const& rhs)
{
	return Dual(rhs.real + x, rhs.dual + x);
}

inline Dual operator-(int x, Dual const& rhs)
{
	return Dual(rhs.real - x, rhs.dual - x);
}
inline Dual operator+(int x, Dual const& rhs)
{
	return Dual(rhs.real + x, rhs.dual + x);
}


//// --- DIFFERENTIATION FUNCTIONS --- ////

inline float diff(Dual(*f) (Dual arg), float x)
{
	return f(Dual(x, 1)).dual;
}


inline vector3 diff(Dual(*f)(Dual arg), vector3 x)
{
	return vector3{ f(Dual(x.v[0], 1)).dual, f(Dual(x.v[1], 1)).dual, f(Dual(x.v[2], 1)).dual };
}

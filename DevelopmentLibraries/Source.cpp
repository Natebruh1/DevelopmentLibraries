#include <iostream>
#include "EnumFlags.h"
#include "MathVector.h"
#include "MathDerivative.h"
//EnumFlag auto g=Colours::Red;
int x = 0;


Dual testDer(Dual x)
{
	return x*x + 3*x;
}

int main()
{
	Colours g = Colours::Red|Colours::Blue;
	if (x) [[likely]]
	{

	}
	//std::cout << (int)g << std::endl;


	vector3 a = { 2.f,12.f,8.f };
	vector3 b = { 8,1,3 };
	std::cout << (a*b).x << std::endl;


	
	std::cout << diff(&testDer, a).x;

	return 0;
}


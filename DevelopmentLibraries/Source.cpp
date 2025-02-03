#include <iostream>
#include "EnumFlags.h"
#include "MathVector.h"
//EnumFlag auto g=Colours::Red;
int x = 0;
int main()
{
	Colours g = Colours::Red|Colours::Blue;
	if (x) [[likely]]
	{

	}
	//std::cout << (int)g << std::endl;


	vector3 a = { 4,12,8 };
	vector3 b = { 8,1,3 };
	std::cout << (a*b).x << std::endl;

	return 0;
}
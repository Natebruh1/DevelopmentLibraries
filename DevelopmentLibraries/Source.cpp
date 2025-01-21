#include <iostream>
#include "EnumFlags.h"
//EnumFlag auto g=Colours::Red;

int main()
{
	Colours g = Colours::Red|Colours::Blue;

	std::cout << (int)g << std::endl;
	return 0;
}
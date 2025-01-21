#include <iostream>
#include "EnumFlags.h"
//EnumFlag auto g=Colours::Red;

int main()
{
	auto g = Colours::Red|Colours::Blue;

	std::cout << (int)g << std::endl;
	return 0;
}
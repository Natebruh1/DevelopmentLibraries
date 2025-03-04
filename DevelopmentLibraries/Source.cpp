#include <iostream>
#include "EnumFlags.h"
#include "MathVector.h"
#include "MathDerivative.h"
#include "Erased.h"
//EnumFlag auto g=Colours::Red;
int x = 0;


struct Draw
{
	static void invoker(const auto& self, std::string& a)
	{
		self.draw(a);
	}
	// not necessary, but makes the client code easier to write
	void draw(this const auto& erased, auto& stream)
	{
		erased.invoke(Draw{}, stream);
	}
};

using Drawable = erased::erased<Draw>;

struct Circle {
	void draw(std::string& s) const { std::cout << s; }
};
struct Square {
	void draw(std::string& s) const { std::cout << "Square" << std::endl;; }
};


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

	std::string strIn = ("Input");
	Drawable drawable = Circle{};
	drawable.draw(strIn);
	drawable = Square{};
	drawable.draw(strIn);
	return 0;
}


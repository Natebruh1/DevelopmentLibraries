#include <iostream>
#include "EnumFlags.h"
#include "MathVector.h"
#include "MathDerivative.h"
#include "ECS.h"
//EnumFlag auto g=Colours::Red;
int x = 0;


Dual testDer(Dual x)
{
	return x*x + 3*x;
}


struct Transform {
	float x = 0, y = 0;
};

struct Velocity {
	float vx = 0, vy = 0;
};


int main()
{
	//Enum Flags
	Colours notGreen = Colours::Red|Colours::Blue; //Flags notGreen as Red=true and Blue=true
	if (x) [[likely]]
	{

	}
	//std::cout << (int)g << std::endl;

	//Math Vector
	vector3 a = { 2.f,12.f,8.f };
	vector3 b = { 8,1,3 };
	std::cout << (a*b).x << std::endl;


	//Derivateives
	std::cout << diff(&testDer, a).x<<std::endl;


	//Entity Component System

	ECS ecs;

	Entity e1 = ecs.create_entity();
	ecs.add_component(e1, Transform{ 0, 0 });
	ecs.add_component(e1, Velocity{ 1, 1 });

	Entity e2 = ecs.create_entity();
	ecs.add_component(e2, Transform{ 10, 10 });
	ecs.add_component(e2, Velocity{ 1, 4 }); //Comment out this line and you will see that e2 will not be moved because it does not have a Velocity component.

	ecs.each<Transform, Velocity>([](Entity e, Transform& t, Velocity& v) //Component interaction (and iteration)
		{
		t.x += v.vx;
		t.y += v.vy;
		std::cout << "Entity " << e << " moved to (" << t.x << ", " << t.y << ")"<<std::endl;
		});


	return 0;
}


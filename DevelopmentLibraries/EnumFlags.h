#pragma once
#include <concepts>
#include <iostream>

//EXAMPLES
enum class Colours{Red=1, Blue=2, Green=4 ,EnumFlagEnabled=8};
enum class State{MOVING,IDLE};
//EXAMPLES END


template<typename t>
concept EnumFlag = requires (){ t::EnumFlagEnabled; };


template<EnumFlag t>
t operator|(t a, t b)
{
	return static_cast<t>(static_cast<int>(a) | static_cast<int>(b));
}
#pragma once
#include <concepts>
#include <iostream>

//EXAMPLES
enum class Colours{Red=1, Blue=2, Green=4 ,EnumFlagEnabled=8};	//EnumFlagEnabled is a flag that indicates this enum can be used with operator| as a flag
enum class State{MOVING,IDLE};	//State does not have EnumFlagEnabled, so it cannot be used with operator| as a flag
//EXAMPLES END


template<typename t>
concept EnumFlag = requires (){ t::EnumFlagEnabled; };


template<EnumFlag t>
t operator|(t a, t b)
{
	return static_cast<t>(static_cast<int>(a) | static_cast<int>(b));
}
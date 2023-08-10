module;

#include "TestClass.hpp"

export module module2;

export using Y = Test::SomeClass;
export using Test::SomeClass; // has to be consumed as SomeClass

export namespace Test2
{
	using Test::SomeClass;
}

module; // everything below is part of the implicit global module fragment

#include <iostream>

module LibModule; // everything below belongs to LibModule module

namespace TestNamespace
{
	Test2::Test2() { }

	void Test2::Print()
	{
		InternalPrint();
	}

	ITest3::~ITest3() {};
}

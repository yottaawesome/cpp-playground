module; // everything below is part of the implicit global module fragment

#include <iostream>

module LibModule; // everything below belongs to LibModule module

namespace TestNamespace
{
	void MyFunc()
	{
		std::wcout << L"Hello World from TestNamespace::MyFunc()!" << std::endl;
	}

	Test::Test() { }

	void Test::Print()
	{
		InternalPrint();
	}

	void InternalPrint()
	{
		std::wcout << L"Hello World from TestNamespace::Test!" << std::endl;
	}
}

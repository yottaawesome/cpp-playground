#include <iostream>

import LibModule;
import DllModule;

int main(int argc, char** args)
{
	TestNamespace::MyFunc();
	TestNamespace::Test t;
	t.Print();
	DllClass dll;
	dll.Hello();
}

#define WIN32_LEAN_AND_MEAN

// https://github.com/microsoft/STL/issues/4343
// Weird bug, the std.ixx.obj doesn't appear to be built.
// Need to manually link it in the Linker > Additional Dependencies
// setting to point to the DLL version.
//#include <iostream>
//#include <memory>
//#include <format>
#include <Windows.h>

import std;
import LibModule;
import DllModule;
import TestModule;
import ModuleNamespace;
import somemodule;

int main(int argc, char** args)
{
	int someF = NamespaceTest::Get();
	std::string s = NamespaceTest::GetAString();
	ERT::XX k;
	k.MM();

	// Here we use the exported class from the DLL
	AA a;
	a.MM();
	a.NN();
	a.YY();
	AA::YY();
	try
	{
		a.OO();
	}
	catch(...)
	{
		std::wcout << "Caught exception\n";
	}

	// Here we use TestFunc from ExecutableModule
	TestModule::TestFunc();

	// Here we use the module from the static lib
	TestNamespace::MyFunc();
	TestNamespace::Test t;
	t.Print();

	// Here we use the exported class from the DLL
	DllClass dll;
	dll.Hello();
	Another::Sup();
	HMODULE dllModule = GetModuleHandleW(L"DllModule");
	if (!dllModule)
	{
		std::wcout << L"Failed getting DllModule HMODULE\n";
		return 1;
	}

	using returnNumberFromDLL = int(*)();
	returnNumberFromDLL returnNumber = reinterpret_cast<returnNumberFromDLL>(
		GetProcAddress(dllModule, "returnNumber")
	);
	if (!returnNumber)
	{
		std::wcout << L"Failed getting returnNumber() pointer\n";
		return 1;
	}

	std::wcout << std::format(
		L"Successfully invoked returnNumber(): {}", 
		returnNumber()
	);

	return 0;
}

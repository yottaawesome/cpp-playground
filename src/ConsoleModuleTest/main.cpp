#include <iostream>
#include <memory>
#include <format>
#include <Windows.h>

import LibModule;
import DllModule;
import TestModule;

int main(int argc, char** args)
{
	ERT::XX k;
	k.MM();

	// Here we use the exported class from the DLL
	AA a;
	a.MM();

	// Here we use TestFunc from ExecutableModule
	TestModule::TestFunc();

	// Here we use the module from the static lib
	TestNamespace::MyFunc();
	TestNamespace::Test t;
	t.Print();

	// Here we use the exported class from the DLL
	DllClass dll;
	dll.Hello();

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

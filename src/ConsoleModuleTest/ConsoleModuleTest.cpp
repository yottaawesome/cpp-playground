#include <iostream>
#include <Windows.h>

import LibModule;
import DllModule;

typedef int (*fnDllModule2Ptr)();

int main(int argc, char** args)
{
	TestNamespace::MyFunc();
	TestNamespace::Test t;
	t.Print();
	DllClass dll;
	dll.Hello();

	HMODULE hmod = nullptr;
	hmod = GetModuleHandleW(L"DllModule");
	//GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, L"DllModule", &hmod);
	if (hmod)
		std::wcout << L"OK" << std::endl;
	fnDllModule2Ptr x = (fnDllModule2Ptr)GetProcAddress(hmod, "fnDllModule2");
	if (x)
		std::wcout << L"OK " << x() << std::endl;
}

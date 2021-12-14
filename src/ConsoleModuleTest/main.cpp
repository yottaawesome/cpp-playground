#include <iostream>
#include <memory>
#include <Windows.h>

import LibModule;
import DllModule;

typedef int (*fnDllModule2Ptr)();

template<typename T>
using releasable_unique_ptr = std::unique_ptr<T, void(*)(T*)>;

using int_unique = releasable_unique_ptr<int>;

int main(int argc, char** args)
{
	AA a;
	F x1;

	releasable_unique_ptr<int>(new int(1), [](int* value) { delete value; });
	int_unique(new int(1), [](int* ptr) { delete ptr; });

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
	return 0;
}

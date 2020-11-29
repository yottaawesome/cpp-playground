module;

#include "pch.h"
#include "framework.h"
#include <iostream>

module DllModule;

// This is an example of an exported function.
int fnDllModule()
{
    return 0;
}

void DllClass::Hello()
{
    std::wcout << L"Hello from DllClass!" << std::endl;
}

module;

#include <iostream>
#include <Windows.h>

module DllModule;

void DllClass::Hello()
{
    std::wcout << L"Hello from DllClass!" << std::endl;
}
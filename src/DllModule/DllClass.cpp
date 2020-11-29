module;

#include "pch.h"
#include "framework.h"
#include <iostream>

module DllModule;

void DllClass::Hello()
{
    std::wcout << L"Hello from DllClass!" << std::endl;
}
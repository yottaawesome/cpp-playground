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

int returnNumber() { return 0; }

namespace ERT
{
	void XX::MM() {};
}
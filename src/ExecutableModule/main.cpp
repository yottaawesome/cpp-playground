#include <iostream>
import TestModule;

int main()
{
    std::cout << "Hello World!\n";
    TestModule::TestStruct ts;
    TestModule::Blah ts2;
    TestModule::TestFunc2();
    TestModule::Alias value = 5;

    return 0;
}

#include <iostream>
import TestModule;

int main()
{
    TestModule::TestStruct ts;
    TestModule::Blah ts2;
    TestModule::TestFunc2();
    TestModule::Alias value = 5;

    TestModule::Partial p2;
    TestModule::Partial* p = TestModule::GetPartial();
    p->Blah();
    
    //TestModule::Partial2 p3;
    TestModule::PtrPartial2 p3 = TestModule::GetPartial2();

    std::unique_ptr p4 = TestModule::GetPartial3();
    p4->Blah();

    return 0;
}

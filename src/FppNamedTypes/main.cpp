// Adapted from FluentC++'s NamedTypes sample: https://github.com/joboccara/NamedType
// Main changes are just removal of macros as this is just for testing the concept
// on MSVC. Also added a module-based implementation of Fluent's code.

#include <iostream>
#include <string>
#include "include/named_type.hpp"
import strongtypes;

void Fluent()
{
    using Speed = fluent::NamedType<int, struct SpeedTag, fluent::Addable, fluent::Subtractable>;
    using Label = fluent::NamedType<std::string, struct LabelTag, fluent::Addable>;

    Label l1("sss");
    Label l2("aaa");
    l1 += l2;

    Speed s1(1);
    Speed s2(4);
    Speed s3 = s1 + s2;

    std::cout << s3.get();
}

void Module()
{
    using Speed = StrongTypes::NamedType<int, struct SpeedTag, StrongTypes::Addable, StrongTypes::Subtractable>;
    using Label = StrongTypes::NamedType<std::string, struct LabelTag, fluent::Addable>;

    Label l1("aaa");
    Label l2("sss");
    l1 += l2;

    Speed s1(1);
    Speed s2(4);
    Speed s3 = s1 + s2;

    std::cout << s3.get() << "\n";
    std::cout << l1.get() << "\n";
}

int main()
{
    Module();
    return 0;
}

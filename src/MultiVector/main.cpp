#include <iostream>

import multivector;

struct A
{
    void DoSomething() { std::cout << "Hello from A!\n"; }
};
struct B
{
    void DoSomething() { std::cout << "Hello from B!\n"; }
};
struct C
{
    void DoSomething() { std::cout << "Hello from C!\n"; }
};

int main()
{
    multivector<A, B, C> vectorOfThings;
    vectorOfThings.push_back(A{});
    vectorOfThings.push_back(B{});
    vectorOfThings.push_back(C{});

    vectorOfThings.for_all(
        [](auto& item)
        {
            item.DoSomething();
        }
    );

    return 0;
}
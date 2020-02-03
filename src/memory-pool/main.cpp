#include <iostream>
#include <utility>
#include "MemoryPool.hpp"

class A
{
public:
    A() : i(1), t(2) {}
    A(int i, short t) : i(i), t(t) {}
    int i;
    short t;
};

int main(int argc, char* args)
{
    MemoryPool op(10000);
    A* a = op.Allocate<A>(1, 5);
    std::cout << a->i << std::endl;
    //A* b = op.Allocate<A>(2, 7);
    //int* i = op.Allocate<int>(1);
    //op.Deallocate(i);

    //std::cout << sizeof(wchar_t) << std::alignment_of<wchar_t>::value << std::endl;
    //std::cout << b->i << " " << a->t << std::endl;
    return 0;
}

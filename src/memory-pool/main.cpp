#include <iostream>
#include <utility>
#include <memory>
#include <stdexcept>
#include "MemoryPool.hpp"

template<typename T>
class required_shared_ptr
{
    public:
        virtual T* operator->() const
        {
            T* value = _internal.operator->();
            if (value == nullptr)
                throw std::runtime_error("value is nullptr");
            return value;
        }

        virtual required_shared_ptr<T>& operator=(T* other)
        {
            _internal.reset(other);
            return *this;
        }

        virtual required_shared_ptr<T>& operator=(required_shared_ptr<T>& other)
        {
            _internal = other._internal;
            return *this;
        }

    private:
        std::shared_ptr<T> _internal;
};

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
    required_shared_ptr<int> test;

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

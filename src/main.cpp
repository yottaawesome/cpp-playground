#include <iostream>
#include <utility>

class A
{
public:
    A() : i(1) {}
    A(int i) : i(i) {}
    int i;
};

class ObjectPool
{
    public:
        ObjectPool(int size) : buffer(new char[size]), offset(0) {}
        virtual ~ObjectPool() { delete[] buffer; }

        template<typename T, typename...Args>
        T* Allocate(Args&&...args)
        {
            offset += sizeof(T);
            return new(buffer+offset) T(std::forward<Args>(args)...);
        }

    private:
        char* buffer;
        int offset;
};

int main()
{
    ObjectPool op(10000);
    A* a = op.Allocate<A>(1);
    A* b = op.Allocate<A>(2);

    std::cout << b->i << " " << a->i << std::endl;
}


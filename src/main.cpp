#include <iostream>
#include <utility>

class A
{
public:
    A() : i(1), t(2) {}
    A(int i, short t) : i(i), t(t) {}
    int i;
    short t;
};

class MemoryPool
{
    public:
        MemoryPool(int size) : buffer(new char[size]), offset(0) {}
        virtual ~MemoryPool() { delete[] buffer; }

        template<typename T, typename...Args>
        T* Allocate(Args&&...args)
        {
            std::cout << "alignment: " << std::alignment_of<T>::value << " sizeof: " << sizeof(T) << std::endl;
            offset += sizeof(T);
            return new(buffer+offset) T(std::forward<Args>(args)...);
        }

    private:
        char* buffer;
        int offset;
};

int main()
{
    MemoryPool op(10000);
    A* a = op.Allocate<A>(1, 5);
    A* b = op.Allocate<A>(2, 7);
    op.Allocate<int>(1);

    std::cout << sizeof(wchar_t) << std::alignment_of<wchar_t>::value << std::endl;
    std::cout << b->i << " " << a->t << std::endl;
}


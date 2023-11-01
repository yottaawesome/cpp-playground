#include <windows.h>
import std;
import std.compat;

template<typename T>
class Q
{
    friend T;

    public:
        Q(T& t) : m_t(t) {}

    public:
        void DoThis() { m_t.DoSomething(*this); }

    private:
        int M;
        T m_t;
};

template<auto T>
class Q2
{
    friend decltype(T);

    public:
        void DoThis() { T.DoSomething(*this); }

    private:
        int M;
};

class M
{
    void DoSomething(Q<M>& m)
    {
        m.M = 1;
    }
};

int main()
{
    M m;
    Q<M> qm(m);
    return 0;
}

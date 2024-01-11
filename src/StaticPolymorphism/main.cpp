import std;
import crtp;
import ducktyping;

// See also https://en.cppreference.com/w/cpp/language/crtp
namespace AnotherExample
{
    struct Base 
    { 
        void name(this auto&& self) { self.impl(); } 
    };

    struct D1 : public Base 
    { 
        void impl() { std::print("D1::impl()"); } 
    };

    struct D2 : public Base 
    { 
        void impl() { std::print("D2::impl()"); } 
    };

    void test()
    {
        D1 d1; 
        d1.name();
        D2 d2; 
        d2.name();
    }

    void Run()
    {
        test();
    }
}

int main() 
{
    return CRTP::Run();
    return DuckTyping::Run();
}
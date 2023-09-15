module;

#include <iostream>

export module crtp;

// Adapted from https://www.modernescpp.com/index.php/c-is-still-lazy
// See also https://en.cppreference.com/w/cpp/language/crtp
// crtp.cpp
export namespace CRTP
{
    template <typename Derived>
    struct Base 
    {
        // New syntax, but MSVC is dumb and ICEs out
        /*void interface(this auto&& self)
        {
            self.implementation();
        }*/
        
        // traditional
        void interface()
        {
            static_cast<Derived*>(this)->implementation();
        }

        void implementation() 
        {
            std::cout << "Implementation Base" << std::endl;
        }
    };

    struct Derived1 : public Base<Derived1>
    {
        void implementation() 
        {
            std::cout << "Implementation Derived1" << std::endl;
        }
    };

    struct Derived2 : public Base<Derived2>
    {
        void implementation() 
        {
            std::cout << "Implementation Derived2" << std::endl;
        }
    };

    struct Derived3 : public Base<Derived3> 
    {
        void implementation()
        {
            std::cout << "Implementation Derived3" << std::endl;
        }
    };

    template <typename T>
    void execute(T& base) 
    {
        base.interface();
    }

	int Run()
	{
        std::cout << std::endl;

        Derived1 d1;
        execute(d1);

        Derived2 d2;
        execute(d2);

        Derived3 d3;
        execute(d3);

        std::cout << std::endl;
        return 0;
	}
}
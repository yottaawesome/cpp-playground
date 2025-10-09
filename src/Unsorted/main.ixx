export module main;
import std;

// Adapted from the code and explanations here:
// https://en.cppreference.com/w/cpp/language/rule_of_three.html

namespace RuleOfZero 
{
    struct rule_of_zero
    {
        std::string cppstring;
        rule_of_zero(const std::string& arg) : cppstring(arg) {}
    };
}
namespace RuleOfThree 
{
    // If a class requires a user-defined destructor, a user-defined 
    // copy constructor, or a user-defined copy assignment operator, 
    // it almost certainly requires all three. 
    struct rule_of_three
    {
        char* cstring; // raw pointer used as a handle to a
        // dynamically-allocated memory block

        explicit rule_of_three(const char* s = "") : cstring(nullptr)
        {
            if (s)
            {
				auto size = std::strlen(s) + 1;
                cstring = new char[size]; // allocate
                std::copy_n(s, size, cstring); // populate
            }
        }

        ~rule_of_three() // I. destructor
        {
            delete[] cstring; // deallocate
        }

        rule_of_three(const rule_of_three& other) // II. copy constructor
            : rule_of_three(other.cstring) {
        }

        rule_of_three& operator=(const rule_of_three& other) // III. copy assignment
        {
            // implemented through copy-and-swap for brevity
            // note that this prevents potential storage reuse
            rule_of_three temp(other);
            std::swap(cstring, temp.cstring);
            return *this;
        }

        const char* c_str() const // accessor
        {
            return cstring;
        }
    };

    void Run()
    {
        rule_of_three o1{ "abc" };
        std::cout << o1.c_str() << ' ';
        auto o2{ o1 }; // II. uses copy constructor
        std::cout << o2.c_str() << ' ';
        rule_of_three o3("def");
        std::cout << o3.c_str() << ' ';
        o3 = o2; // III. uses copy assignment
        std::cout << o3.c_str() << '\n';
    }   // I. all destructors are called here
}
namespace RuleOfFive 
{
    /*
    * Because the presence of a user-defined (include = default or = delete declared) 
    * destructor, copy-constructor, or copy-assignment operator prevents implicit 
    * definition of the move constructor and the move assignment operator, any class 
    * for which move semantics are desirable, has to declare all five special member 
    * functions: 
    */
    struct rule_of_five
    {
        char* cstring; // raw pointer used as a handle to a
        // dynamically-allocated memory block
        explicit rule_of_five(const char* s = "") : cstring(nullptr)
        {
            if (s)
            {
                auto size = std::strlen(s) + 1;
                cstring = new char[size]; // allocate
                std::copy_n(s, size, cstring); // populate
            }
        }

        ~rule_of_five()
        {
            delete[] cstring; // deallocate
        }

        rule_of_five(const rule_of_five& other) // copy constructor
            : rule_of_five(other.cstring) {
        }

        rule_of_five(rule_of_five&& other) noexcept // move constructor
            : cstring(std::exchange(other.cstring, nullptr)) {
        }

        rule_of_five& operator=(const rule_of_five& other) // copy assignment
        {
            // implemented as move-assignment from a temporary copy for brevity
            // note that this prevents potential storage reuse
            return *this = rule_of_five(other);
        }

        rule_of_five& operator=(rule_of_five&& other) noexcept // move assignment
        {
            std::swap(cstring, other.cstring);
            return *this;
        }

        // alternatively, replace both assignment operators with copy-and-swap
        // implementation, which also fails to reuse storage in copy-assignment.
        //  rule_of_five& operator=(rule_of_five other) noexcept
        //  {
        //      std::swap(cstring, other.cstring);
        //      return *this;
        //  }
    };

    void Run()
    {
    }
}

extern "C" int main()
{
	return 0;
}

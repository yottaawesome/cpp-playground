import std;

struct Type
{
    ~Type() { std::println("Destructor"); }
    Type() { std::println("Default constructor"); }
    Type(const Type&) { std::println("Copy constructor"); };
    Type& operator=(const Type&) { std::println("Copy assignment"); return *this; };
    Type(Type&&) { std::println("Move constructor"); };
    Type& operator=(Type&&) { std::println("Move assignment"); return *this; };
};

decltype(auto) Get(auto&& s)
{
    return std::forward<decltype(s)>(s);
}

// Adapted from https://medium.com/@barryrevzin/value-categories-in-c-17-f56ae54bccbe
/**
    A glvalue is an expression whose evaluation determines the identity of an object, bit-field, or function.
    A prvalue is an expression whose evaluation initializes an object or a bit-field, or computes the value of the operand of an operator, as specified by the context in which it appears.
    An xvalue is a glvalue that denotes an object or bit-field whose resources can be reused (usually because it is near the end of its lifetime).
*/

namespace detail {
    template <class T> struct value_category { static constexpr char const* value = "prvalue"; };
    template <class T> struct value_category<T&> { static constexpr char const* value = "lvalue"; };
    template <class T> struct value_category<T&&> { static constexpr char const* value = "xvalue"; };
}

#define PRINT_VALUE_CAT(expr) std::cout << #expr << " is a " << ::detail::value_category<decltype((expr))>::value << '\n'

struct S { int i; };

int main()
{
    int&& r = 42;
    PRINT_VALUE_CAT(4); // prvalue
    PRINT_VALUE_CAT(r); // lvalue
    PRINT_VALUE_CAT(std::move(r)); // xvalue

    PRINT_VALUE_CAT(S{ 0 }); // prvalue
    PRINT_VALUE_CAT(S{ 0 }.i); // xvalue (gcc erroneously calls this a prvalue)
}
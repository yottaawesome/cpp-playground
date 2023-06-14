import std;

// Adapted from https://stackoverflow.com/a/58455135/7448661
template<int FOO_INIT>
struct TestImpl1 
{
    static int foo;
};

template<int FOO_INIT>
int TestImpl1<FOO_INIT>::foo = FOO_INIT;

template<int FOO_INIT>
struct TestImpl2 
{
    constexpr static int foo = FOO_INIT;
};

template<int FOO_INIT>
constexpr int TestImpl2<FOO_INIT>::foo;

template<int size>
struct Test : std::conditional_t<(size < 10), TestImpl1<7>,TestImpl2<12>> {};

int main()
{
    Test<5> v1;
    Test<15> v2;

    std::cout << v1.foo << "\n";
    std::cout << v2.foo << "\n";

    // constexpr int i1 = v1.foo; // Fails to compile because Test<5>::foo is not constexpr.
    constexpr int i2 = v2.foo; // Compiles because Test<15>::foo is constexpr.

    return 0;
}

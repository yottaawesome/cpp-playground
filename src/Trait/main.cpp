import std;
import std.compat;

namespace Test
{
    void M(int i) //[[std::expects: i > 0]]
    {

    }

    // See https://andreasfertig.blog/2021/01/cpp20-aggregate-pod-trivial-type-standard-layout-class-what-is-what/
    // Standard-layout structs are interoperable with C APIs
    struct TestStruct
    {
        // All fields must use the same access modifier
        int M = 1;
        int N = 2;
        // Access modifiers for function members and special member have no effect on the layout of the class,
        // so any combination is permitted.
        ~TestStruct() {}        // allowed, must not be virtual
        TestStruct() {}         // allowed
        TestStruct(int m) {}    // allowed
        void SomeFunction() {}  // allowed, must not be virtual
    };

    void Run()
    {
        std::cout << std::format("std::is_standard_layout: {}", std::is_standard_layout<TestStruct>::value);
    }
}

auto main() -> int
{
    Test::Run();
    return 0;
}

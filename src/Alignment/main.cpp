import std;

struct Something
{
    int Y = 10;
};

// https://stackoverflow.com/questions/20221311/convert-a-struct-to-vector-of-bytes
// https://stackoverflow.com/questions/71828288/why-is-stdaligned-storage-to-be-deprecated-in-c23-and-what-to-use-instead
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1413r3.pdf
int main()
{
    std::cout << "Hello World!\n";

    constexpr auto length = sizeof(Something) + 20;

    std::allocator<std::byte> buffer;

    std::byte* bytes = buffer.allocate(length);
    Something* s = new (bytes) Something();

    alignas(Something) std::byte bb[sizeof(Something)];
    std::copy_n(bytes, sizeof(Something), &bb[0]);

    std::array<Something, 1> bb2;
    std::copy_n(bytes, sizeof(Something), (std::byte*)bb2.data());

    //operator new (8, std::align_val_t{ 5 });

    buffer.deallocate(bytes, length);

    return 0;
}

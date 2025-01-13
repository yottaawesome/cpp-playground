import std;

struct Something
{
    int Y = 10;
};

constexpr void DoSpan(std::span<std::byte> b)
{
    std::fill(b.begin(), b.end(), std::byte{ 0x10 });
}

static_assert(
    []()
    {
        //std::regex ipv4("(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");
        std::byte b[3]{};
        std::string IP{ "127.0.0.1" };
        //bool c = std::regex_match(IP, ipv4);
        //return c;
        DoSpan(b);
        return b[0] == std::byte{ 0x10 } and b[1] == std::byte{ 0x10 } and b[2] == std::byte{ 0x10 };
    }()
);


// https://stackoverflow.com/questions/20221311/convert-a-struct-to-vector-of-bytes
// https://stackoverflow.com/questions/71828288/why-is-stdaligned-storage-to-be-deprecated-in-c23-and-what-to-use-instead
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1413r3.pdf
// https://stackoverflow.com/questions/53922209/how-to-invoke-aligned-new-delete-properly
// https://stackoverflow.com/questions/39058850/how-to-align-stdarray-contained-data
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

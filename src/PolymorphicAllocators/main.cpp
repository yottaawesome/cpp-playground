// Adapted from https://www.modernescpp.com/index.php/special-allocators-with-c17/
#include <array>
#include <cstddef>
#include <format>
#include <memory_resource>
#include <vector>
#include <iostream>

namespace BasicAllocator
{
    void Run()
    {
        std::cout << "Running BasicAllocator...\n";

        std::array<std::byte, 200> buf1;                               // (1)
        std::pmr::monotonic_buffer_resource pool1{ buf1.data(), buf1.size() };
        std::pmr::vector<int> myVec1{ &pool1 };                          // (3)
        for (int i = 0; i < 5; ++i) 
        {
            myVec1.push_back(i);
        }

        char buf2[200] = {};                                           // (2)
        std::pmr::monotonic_buffer_resource pool2{ std::data(buf2), std::size(buf2) };
        std::pmr::vector<int> myVec2{ &pool2 };
        for (int i = 0; i < 200; ++i) 
        {
            myVec2.push_back(i);
        }

        std::cout << "\n";
    }
}

namespace TrackingAllocator
{
    class TrackAllocator : public std::pmr::memory_resource 
    {
        void* do_allocate(std::size_t bytes, std::size_t alignment) override 
        {
            void* p = std::pmr::new_delete_resource()->allocate(bytes, alignment);
            std::cout << std::format("  do_allocate: {:6} bytes at {}\n", bytes, p);
            return p;
        }

        void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override 
        {
            std::cout << std::format("  do_deallocate: {:4} bytes at {}\n", bytes, p);
            return std::pmr::new_delete_resource()->deallocate(p, bytes, alignment);
        }

        bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override 
        {
            return std::pmr::new_delete_resource()->is_equal(other);
        }
    };

    void Run()
    {
        std::cout << "Running TrackingAllocator...\n";

        TrackAllocator trackAllocator;                         // (1)
        std::pmr::set_default_resource(&trackAllocator);       // (2)

        std::cout << "myVec1\n";

        std::array<std::byte, 200> buf1;
        std::pmr::monotonic_buffer_resource pool1{ buf1.data(), buf1.size() };
        std::pmr::vector<int> myVec1{ &pool1 };                  // (3)
        for (int i = 0; i < 5; ++i) 
        {
            myVec1.push_back(i);
        }

        std::cout << "myVec2\n";

        char buf2[200] = {};
        std::pmr::monotonic_buffer_resource pool2{ std::data(buf2), std::size(buf2) };
        std::pmr::vector<int> myVec2{ &pool2 };                  // (4)
        for (int i = 0; i < 200; ++i) 
        {
            myVec2.push_back(i);
        }

        std::cout << "TrackingAllocator allocator finished -- memory will now be destroyed.\n";
    }
}

namespace NonAllocatingAllocator
{
    void Run()
    {
        std::cout << "\nRunning NonAllocatingAllocator...\n";

        std::array<std::byte, 2000> buf;
        std::pmr::monotonic_buffer_resource pool{ buf.data(), buf.size(),  // (1)
                                               std::pmr::null_memory_resource() };
        std::pmr::vector<std::pmr::string> myVec{ &pool };                  // (2)
        try 
        {
            for (int i = 0; i < 100; ++i) 
            {                               // (3)
                std::cerr << i << " ";
                myVec.emplace_back("A short string");
            }
        }
        catch (const std::bad_alloc& e) 
        {                                 // (4)
            std::cerr << '\n' << e.what() << '\n';
        }

        std::cout << "\n";
    }
}

int main()
{
    BasicAllocator::Run();
    TrackingAllocator::Run();
    NonAllocatingAllocator::Run();
    return 0;
}

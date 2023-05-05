module;

// Adapted from https://www.modernescpp.com/index.php/ranges-improvements-with-c-23
// range.cpp

#include <iostream>
#include <ranges>
#include <vector>
#include <algorithm>

export module ranges;

export namespace RangesA
{
	int Run();
}

export namespace RangesB
{
    int Run();
}

module :private;

namespace RangesA
{
    std::vector<int> range(int begin, int end, int stepsize = 1) 
    {
        std::vector<int> result{};
        if (begin < end) 
        {                                     // (5)
            auto boundary = [end](int i) { return i < end; };
            for (
                int i : std::ranges::views::iota(begin) 
                | std::ranges::views::stride(stepsize)
                | std::ranges::views::take_while(boundary)) 
            {
                result.push_back(i);
            }
        }
        else 
        {                                                 // (6)
            begin++;
            end++;
            stepsize *= -1;
            auto boundary = [begin](int i) { return i < begin; };
            for (
                int i : std::ranges::views::iota(end) 
                | std::ranges::views::take_while(boundary)
                | std::ranges::views::reverse
                | std::ranges::views::stride(stepsize)) 
            {
                result.push_back(i);
            }
        }
        return result;
    }

    int Run() 
    {
        std::cout << "Running sample A\n";

        // range(1, 50)                                       // (1)
        auto res = range(1, 50);
        for (auto i : res) std::cout << i << " ";

        std::cout << "\n\n";

        // range(1, 50, 5)                                    // (2)
        res = range(1, 50, 5);
        for (auto i : res) std::cout << i << " ";

        std::cout << "\n\n";

        // range(50, 10, -1)                                  // (3)
        res = range(50, 10, -1);
        for (auto i : res) std::cout << i << " ";

        std::cout << "\n\n";

        // range(50, 10, -5)                                  // (4)
        res = range(50, 10, -5);
        for (auto i : res) std::cout << i << " ";

        std::cout << "\nSample A finished.\n\n";

        return 0;
    }
}

namespace RangesB
{
    int Run()
    {
        std::cout << "Running sample B\n";

        // A vector we want to filter
        std::vector a{ 1,2,3,4,5 };
        // Our filter view
        auto filterView = 
            a | std::ranges::views::filter([](const int a) { return a % 2; });
        // Copy the correct elements to a new vector

        std::vector b(filterView.begin(), filterView.end());
        // Which is equivalent to
        std::vector<int> c;
        std::ranges::copy(filterView, std::back_inserter(c));
        // Which is equivalent to
        std::vector<int> d;
        std::ranges::copy_if(a, std::back_inserter(d), [](int x) { return x % 2; });
        // Just print the laste vector
        std::ranges::copy(d, std::ostream_iterator<int>(std::cout, " "));

        std::cout << "\nSample B finished.\n\n";

        return 0;
    }
}
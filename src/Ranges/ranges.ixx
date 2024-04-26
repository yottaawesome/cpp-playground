// Adapted from https://www.modernescpp.com/index.php/ranges-improvements-with-c-23
// range.cpp
export module ranges;
import std;

export namespace RangesA
{
	int Run();
}

export namespace RangesB
{
    int Run();
}

export namespace RangesWithFile
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

namespace RangesWithFile
{
    // adapted from https://mobiarch.wordpress.com/2023/12/17/reading-a-file-line-by-line-using-c-ranges/
    struct file_line
    {
        std::string line;
        size_t line_number = 0;
 
        friend std::istream &operator>>(std::istream& s, file_line& fl)
        {
            std::getline(s, fl.line);
 
            ++fl.line_number;
 
            return s;
        }
    };

    int Run()
    {
        std::filesystem::path p{ "cube.obj" };
        if (not std::filesystem::exists(p))
            throw std::runtime_error("Path not found");

        std::ifstream file;
        file.open(p.string());
        if (file.fail())
            throw std::runtime_error("Stream in bad state");

        std::vector<std::string> vertexLines;
        std::vector<std::string> faceLines;
        auto filter =
            std::views::istream<file_line>(file)
            | std::views::filter([](file_line& s) { return s.line.starts_with("v ") or s.line.starts_with("f "); });
        for (const auto& fl : filter)
            ;

        std::vector<std::string> m { "b", "baa" };

        auto s = m 
            | std::views::filter([](auto& s) { return s == "a"; }) 
            | std::views::take(1) 
            | std::views::join 
            | std::ranges::to<std::string>();

        return 0;
    }
}
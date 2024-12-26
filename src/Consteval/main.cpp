import std;

namespace Consteval1
{
    struct Q
    {
        consteval Q() { I.push_back(1); }
        std::vector<int> I;
    };

    consteval std::string_view A()
    {
        Q q;
        int* x = new int(1);
        delete x;
        Q* y = new Q();
        delete y;
        if (q.I.front() == 1)
            return "AA";
        else
            return "BB";
    }

    auto Run() -> auto
    {
        constexpr std::string_view m = A();
    }
}

namespace Consteval2
{
    // std::sqrt is constexpr in C++26, which MSVC does not yet support.
    // Adapted from https://gist.github.com/alexshtf/eb5128b3e3e143187794
    constexpr double sqrtNewtonRaphson(double x, double curr, double prev)
    {
        return curr == prev
            ? curr
            : sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
    }

    constexpr double SquareRoot(double x)
    {
        return x >= 0 && x < std::numeric_limits<double>::infinity()
            ? sqrtNewtonRaphson(x, x, 0)
            : std::numeric_limits<double>::quiet_NaN();
    }

    constexpr double Square(double x)
    {
        return x * x;
    }

    struct Point
    {
        consteval Point(int x, int y)
            : X(x), Y(y) { }
        int X = 0;
        int Y = 0;
    };

    struct Line
    {
        consteval Line(Point a, Point b)
            : A(a), B(b) { }
        Point A;
        Point B;
        consteval auto Length() noexcept -> float
        {
            // rise over run
            double rise = Square(static_cast<double>(B.Y - A.Y));
            double run = Square(static_cast<double>(B.X - A.X));
            return static_cast<float>(SquareRoot(rise + run));
        }
    };

    constexpr auto GetLines() -> std::vector<Line>
    {
        std::vector<Line> p;
        p.push_back({{ 0, 0 }, { 3, 0 }});
        return p;
    }

    consteval auto GetTotalLength() -> float
    {
        auto lines = GetLines();
        if (lines.empty())
            throw std::exception("Cannot be empty.");
        float length = 0;
        for (Line l : lines)
            length += l.Length();
        return length;
    }

    auto Run() -> void
    {
        constexpr float totalLength = GetTotalLength();
        std::println("The computed linear line length of all lines is {} units.", totalLength);
    }
}

auto main() -> int
{
    Consteval2::Run();
    return 0;
}

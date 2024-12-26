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
    };

    constexpr auto GetLines() -> std::vector<Line>
    {
        std::vector<Line> p;
        p.push_back({{ 1, 2 }, { 2, 3 }});
        return p;
    }

    consteval auto GetTotalLength() -> std::uint32_t
    {
        std::uint32_t length = 0;
        auto lines = GetLines();
        if (lines.empty())
            throw std::exception("Cannot be empty.");
        for (Line l : lines) // rise over run
            length += (l.B.Y - l.A.Y) / (l.B.X - l.A.X);
        return length;
    }

    auto Run() -> void
    {
        constexpr int totalLength = GetTotalLength();
        std::println("The length is {}.", totalLength);
    }
}

auto main() -> int
{
    Consteval2::Run();
    return 0;
}

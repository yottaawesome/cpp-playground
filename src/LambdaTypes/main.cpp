import std;
import sample;
import weirdness;

int x() { return 1; }

static_assert(std::is_same_v<int, decltype(x())>, "No");

template<typename T>
concept Exception = std::is_base_of_v<std::exception, std::remove_cvref_t<T>>;

struct S : public std::exception {};
struct T : public std::exception {};

static_assert(Exception<T>);

template<Exception TEx>
struct std::formatter<TEx, char> : std::formatter<char, char>
{
    template<typename TContext>
    auto format(TEx&& object, TContext&& ctx) const
    {
        return format_to(ctx.out(), "{}", object.what());
    }
};

namespace Logging
{
    template<typename...TArgs>
    struct Test
    {
        std::format_string<TArgs...> Fmt;
        consteval Test(auto&& fmt) : Fmt{ fmt } {}
    };

    template<typename...TArgs>
    constexpr void Log(Test<std::type_identity_t<TArgs>...> s, TArgs&&... a)
    {

    }

    template<typename...TArgs>
    constexpr void Log(Test<std::type_identity_t<TArgs>...> s, const std::exception& ex, TArgs&&... a)
    {

    }
}


int main() 
{
    Logging::Log("A {}", 1);
    Logging::Log("{}", std::exception{});

    Weirdness::Run();
    A::Test();
    B::Test();
    C::Test();
    D::Test();
    return 0;
}
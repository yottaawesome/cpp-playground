import std;
import sample;
import weirdness;

int x() { return 1; }

static_assert(std::is_same_v<int, decltype(x())>, "No");

template<typename T>
concept Exception = 
    std::same_as<std::exception, T> 
    or std::is_base_of_v<std::exception, std::remove_cvref_t<T>>;

struct S : public std::exception {};
struct T : public std::exception {};

static_assert(Exception<T>);

//template<Exception TEx>
//struct std::formatter<TEx, char> : std::formatter<char, char>
//{
//    template<typename TContext>
//    auto format(TEx&& object, TContext&& ctx) const
//    {
//        return format_to(ctx.out(), "{}", object.what());
//    }
//};

using Map = std::map<std::string, std::vector<double>>;
auto GetMap()       -> Map;
auto GetAndSetMap() -> Map;

namespace Logging
{
    template<typename...TArgs>
    struct Test
    {
        std::format_string<TArgs...> Fmt;
        consteval Test(auto&& fmt) : Fmt{ fmt } {}
    };

    template<typename THead, typename...TTail>
    consteval bool Except() { return std::convertible_to<THead, std::exception>; }


    template<typename...TArgs>
    constexpr void Log(Test<std::type_identity_t<TArgs>...> s, TArgs&&... a)
        requires (not Except<TArgs...>() or sizeof...(a) == 0)
    {
        auto f = std::format(s.Fmt, std::forward<TArgs>(a)...);
    }

    template<typename...TArgs>
    constexpr void Log(Test<std::type_identity_t<TArgs>...> s, const std::exception& ex, TArgs&&... a)
    {
        auto f = std::format(s.Fmt, std::forward<TArgs>(a)...);
    }
}

template<size_t N>
struct FixedString
{
    char Buffer[N]{};
    consteval FixedString(const char(&buffer)[N]) noexcept
    {
        std::copy_n(buffer, N, Buffer); 
    }

    auto View   (this auto self)    noexcept -> std::string_view    { return self.Buffer; }
    auto String (this auto self)    noexcept -> std::string         { return self.Buffer; }
};

template<FixedString F, typename...TArgs>
concept FormatStringTest =
    [] consteval -> bool
    {
        std::format_string<TArgs...> f{F.Buffer};
        return true;
    }();

template<FixedString F, typename...TArgs>
concept FormatString = 
    [] consteval -> bool
    {
        static_assert(FormatStringTest<F, TArgs...>, "This is not a valid format string.");
        return true;
    }();

int main() 
{
    FormatString<"{} {}", std::string, std::string>;

    [](auto...a)
    {

    }(std::pair{"a", 1});

    Logging::Log("A");
    Logging::Log("A {}, {}", 1, 10);
    Logging::Log("a", std::exception{});
    Logging::Log("{} {}", std::exception{}, "a", 1);

    Weirdness::Run();
    A::Test();
    B::Test();
    C::Test();
    D::Test();
    return 0;
}
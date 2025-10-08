import std;
import sample;
import weirdness;

namespace Logging
{
    template<size_t N>
    struct FixedString
    {
        char Buffer[N]{};
        consteval FixedString(const char(&buffer)[N]) noexcept
        {
            std::copy_n(buffer, N, Buffer);
        }

        auto View(this auto self)    noexcept -> std::string_view { return self.Buffer; }
        auto String(this auto self)    noexcept -> std::string { return self.Buffer; }
    };

    template<FixedString F, typename...TArgs>
    concept FormatStringTest =
        [] { std::format_string<TArgs...> f{ F.Buffer }; return true; }();

    template<FixedString F, typename...TArgs>
    concept FormatString =
        [] { static_assert(FormatStringTest<F, TArgs...>, "This is not a valid format string."); return true; }();

    template<typename...TArgs>
    struct Test
    {
        std::format_string<TArgs...> Fmt;
        consteval Test(auto&& fmt) : Fmt{ fmt }
        {
        }
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

template<auto V>
using Integral = std::integral_constant<int, V>;

int main() 
{
	A::Singleton::get() = std::make_unique<int>(42);
    Logging::FormatString<"{} {}", std::string, std::string>;
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
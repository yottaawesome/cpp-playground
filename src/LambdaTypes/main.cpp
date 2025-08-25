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

template<typename T>
struct Helper
{
    size_t N = std::tuple_size_v<T>;
    static auto S = std::make_index_sequence<std::tuple_size_v<T>>{};

    /*operator std::index_sequence() const noexcept
    {
        return S;
    }*/
};

namespace DoAParticularTest
{
    struct A
    {
        void Do(this auto self)
        {
            std::println("A");
        }
    };
    struct B : A
    {
        void Do(this auto self)
        {
            std::println("B");
        }
    };
    struct C : A
    {
        void Do(this auto self)
        {
            std::println("C");
        }
    };
    template<typename...Ts>
    struct Overload : Ts...
    {
        using Ts::operator()...;
    };

    struct Helper
    {
        std::variant<A, B, C> ABC = B{};
        auto Visit(auto&&...fns)
        {
            std::visit(Overload{ fns... }, ABC);
        }
    };

    struct Operator
    {
        void Do(this auto self)
        {
            //Overload v{ &decltype(self)::Do };

            std::variant<int, float> f = 1.f;
            std::visit(self, f);
            std::visit([self](auto v) { self.Handle(v); }, f);
        }

        void Do2(this auto self)
        {
            //auto x = (decltype(self)::Do2);
            // ICE: Overload v{ Operator::Do2 };
        }

        void operator()(this auto self, auto&& arg) { self.Handle(arg); }
        //static void operator()(this auto self, auto&& arg) {  }

        void Handle(this auto self, int) {}
        void Handle(this auto self, float) {}
    };


    void DoSomething()
    {
        Operator o{};
        o.Do();
        o.Do2();

        Helper h;
        h.Visit(
            [](const A& a) { a.Do(); },
            [](auto&& a) { a.Do(); }
        );

        std::variant<A, B, C> ABC = B{};
        std::visit(
            Overload{ 
                [](this auto&& self, auto a) { a.Do(); } 
            }, 
            ABC
        );
    }
}

template<auto V>
using Integral = std::integral_constant<int, V>;

void Invoke(Integral<1>)
{

}

void Invoke(auto)
{

}

void SomethingElse(int x)
{
    std::tuple constants{
        std::pair { Integral<1>{}, [] { std::println("Hello 1"); }},
        std::pair { Integral<2>{}, [] { std::println("Hello 2"); }}
    };

    std::apply(
        [x](auto...pairs)
        {
            ((pairs.first == x ? (pairs.second(), true) : false) or ...);
        },
        constants
    );
}

struct LL
{
    void Do(this auto&& self)
    {
        self.Get();
    }

    void Print(this auto&& self, int n)
    {
        std::println("Nothing");
    }

    void Something(this auto&& self) {}
};

struct LLL : LL
{
    using LL::Print;

    void Print(this auto&& self, std::integral_constant<int, 1>)
    {
        std::println("Hello");
    }

    void Something(this LLL& self) {}
};

template<typename T>
using M = decltype(&T::Do);


int F(int i)
{
    static int j = 
        [i] 
        {
            std::println("Called {}", i);
            if (i == 0)
                throw std::exception();
            return i; 
        }();
    return j;
}

int main() 
{
    [](auto...i)
    {
        ([i]
        {
            try
            {
                std::println("{}", F(i));
            }
            catch (...)
            {
                std::println("Failure on {}", i);
            }
        }(), ...);
    }(0,1,0);

    constexpr auto k = &LLL::Something;
    LLL l{};
    l.Print(2);
    l.Print(std::integral_constant<int, 1>{});

    SomethingElse(1);

    std::tuple someTuple{ 1, std::string{} };

    [&someTuple]<size_t...M>(std::index_sequence<M...>)
    {
        ([&someTuple]<size_t N = M> 
        { 
            auto value = std::get<N>(someTuple); 
        }(), ...);
    }(std::make_index_sequence<std::tuple_size_v<decltype(someTuple)>>{});

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
module;

#include <iostream>
#include <tuple>
#include <utility>
#include <type_traits>

export module sample;

// Based off https://qiita.com/angeart/items/94734d68999eca575881
export namespace A
{
    namespace stx
    {
        namespace lambda_detail
        {
            template<class Ret, class Cls, class IsMutable, class... Args>
            struct types
            {
                using is_mutable = IsMutable;

                enum { arity = sizeof...(Args) };

                using return_type = Ret;

                template<size_t i>
                struct arg
                {
                    typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
                };
            };
        }

        template<class Ld>
        struct lambda_type
            : lambda_type<decltype(&Ld::operator())>
        {};

        template<class Ret, class Cls, class... Args>
        struct lambda_type<Ret(Cls::*)(Args...)>
            : lambda_detail::types<Ret, Cls, std::true_type, Args...>
        {};

        template<class Ret, class Cls, class... Args>
        struct lambda_type<Ret(Cls::*)(Args...) const>
            : lambda_detail::types<Ret, Cls, std::false_type, Args...>
        {};
    }

    void Test()
    {
        std::cout << "--- Sample A ---\n";
        std::cout << "[is mutable lambda]" << std::endl;
        {
            auto test = [](int a) mutable->long {return static_cast<long>(a); };
            std::cout << "ret type : " << std::is_same<stx::lambda_type<decltype(test)>::return_type, long>::value << std::endl;
            std::cout << "arg size : " << stx::lambda_type<decltype(test)>::arity << std::endl;
            std::cout << "arg 0 type : " << std::is_same<stx::lambda_type<decltype(test)>::arg<0>::type, int>::value << std::endl;
            std::cout << "is mutable : " << std::is_same<stx::lambda_type<decltype(test)>::is_mutable, std::true_type>::value << std::endl;
        }
        std::cout << "[is normal lambda]" << std::endl;
        {
            auto test = [](int a, int b)->long {return static_cast<long>(a); };
            std::cout << "ret type : " << std::is_same<stx::lambda_type<decltype(test)>::return_type, long>::value << std::endl;
            std::cout << "arg size : " << stx::lambda_type<decltype(test)>::arity << std::endl;
            std::cout << "arg 0 type : " << std::is_same<stx::lambda_type<decltype(test)>::arg<0>::type, int>::value << std::endl;
            std::cout << "is mutable : " << std::is_same<stx::lambda_type<decltype(test)>::is_mutable, std::true_type>::value << std::endl;
        }
    }
}

// Based off https://stackoverflow.com/a/19149751/7448661
export namespace B
{
    template< typename t, std::size_t n, typename = void >
    struct function_argument_type;

    template< typename r, typename ... a, std::size_t n >
    struct function_argument_type< r(*)(a ...), n >
    {
        typedef typename std::tuple_element< n, std::tuple< a ... > >::type type;
    };

    template< typename r, typename c, typename ... a, std::size_t n >
    struct function_argument_type< r(c::*)(a ...), n >
        : function_argument_type< r(*)(a ...), n > {};

    template< typename r, typename c, typename ... a, std::size_t n >
    struct function_argument_type< r(c::*)(a ...) const, n >
        : function_argument_type< r(c::*)(a ...), n > {};

    template< typename ftor, std::size_t n >
    struct function_argument_type< ftor, n,
        typename std::conditional< false, decltype(&ftor::operator ()), void >::type >
        : function_argument_type< decltype(&ftor::operator ()), n > {};

    void Test() 
    {
        std::cout << "\n--- Sample B ---\n";
        auto x = [](int, long, bool) {};
        std::cout << typeid(function_argument_type< decltype(x), 0 >::type).name() << '\n';
        std::cout << typeid(function_argument_type< decltype(x), 1 >::type).name() << '\n';
        std::cout << typeid(function_argument_type< decltype(x), 2 >::type).name() << '\n';
    }
}

// https://stackoverflow.com/a/19149568/7448661
export namespace C
{
    template<class T>
    struct decompose;

    template<class Ret, class T, class... Args>
    struct decompose<Ret(T::*)(Args...) const>
    {
        constexpr static int n = sizeof...(Args);
    };

    template<class T>
    int deduce(T t)
    {
        return decompose<decltype(&T::operator())>::n;
    }

    struct test
    {
        void operator() (int) const {}
    };

    void Test()
    {
        std::cout << "\n--- Sample C ---\n";
        std::cout << deduce(test{}) << std::endl;
        std::cout << deduce([](int) {}) << std::endl;
    }
}

// Adapted from https://www.reddit.com/r/cpp_questions/comments/ta9s1t/comment/hzzdekd/?utm_source=share&utm_medium=web2x&context=3
export namespace D
{
    template< class R, class... A >
    struct Func_type_;

    template< class R, class... A >
    struct Func_type_<auto(A...)->R>
    {
        using Return_type = R;
        using Args_tuple = std::tuple< A... >;
    };

    template<class Ret, class T, class... Args>
    struct Func_type_<Ret(T::*)(Args...) const>
    {
        constexpr static int n = sizeof...(Args);
        using Return_type = Ret;
        using Args_tuple = std::tuple< Args... >;
    };

    /*template< class R, class... A >
    struct Func_type_<auto(A...)noexcept(std::is_nothrow_invocable_v<R, A...>)->R>
    {
        using Return_type = R;
        using Args_tuple = std::tuple< A... >;
    };*/

    void foo(int, double);

    void Test()
    {
        std::cout << "\n--- Sample D ---\n";
        auto x = [](int m) {};
        using M = decltype(x);
        using Func_l = Func_type_<decltype(&M::operator())>;
        using T1_l = std::tuple_element_t< 0, Func_l::Args_tuple >;
        std::cout << std::format("{}\n", typeid(T1_l).name());

        using Func = Func_type_<decltype(foo)>;
        using T0 = Func::Return_type;
        using T1 = std::tuple_element_t< 0, Func::Args_tuple >;
        using T2 = std::tuple_element_t< 1, Func::Args_tuple >;
        std::cout << std::format("auto foo({}, {}) -> {}\n", typeid(T1).name(), typeid(T2).name(), typeid(T0).name());
    }
}

// See also https://gist.github.com/Naios/86ccea2f577172c9f8b5
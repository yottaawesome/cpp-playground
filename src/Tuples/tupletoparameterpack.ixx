export module tupletoparameterpack;
import std;

// See https://stackoverflow.com/questions/36612596/tuple-to-parameter-pack
// And https://www.cppstories.com/2022/tuple-iteration-apply/
export namespace DelayedDispatch
{
    template<int ...> 
    struct seq {};

    template<int N, int ...S> 
    struct gens : gens<N - 1, N - 1, S...> { };

    template<int ...S> 
    struct gens<0, S...> { typedef seq<S...> type; };

    double foo(int x, float y, double z)
    {
        return x + y + z;
    }

    template <typename ...Args>
    struct save_it_for_later
    {
        std::tuple<Args...> params;
        double(*func)(Args...);

        double delayed_dispatch()
        {
            return callFunc(typename gens<sizeof...(Args)>::type()); // Item #1
        }

        template<int ...S>
        double callFunc(seq<S...>)
        {
            return func(std::get<S>(params) ...);
        }
    };

    int main()
    {
        std::tuple<int, float, double> t = std::make_tuple(1, (float)1.2, 5);
        save_it_for_later<int, float, double> saved = { t, foo };
        std::cout << saved.delayed_dispatch() << std::endl;
        return 0;
    }
}

export namespace BlabBlah
{
    void BlahBlah(auto&&...args) { }

    template<typename...T>
    void BlahBlahCaller(T&&...t)
    {
        std::apply(BlahBlah<T...>, std::make_tuple(std::forward<T>(t)...));
    }

    template<typename A>
    struct Callable
    {
        template<typename...T>
        void operator()(T&&...args) { }
    };

    int main2()
    {
        BlahBlahCaller(1, 2);
        std::apply(BlahBlah<int, int>, std::make_tuple(1, 2));
        std::apply([](auto &&... args) { BlahBlah(args...); }, std::make_tuple(1, 2));

        return 0;
    }
}

export namespace Strip
{
    // And https://stackoverflow.com/questions/15418841/how-do-i-strip-a-tuple-back-into-a-variadic-template-list-of-types
    template<typename...X>
    struct vct { };

    template<typename>
    struct strip;

    template<typename ...T>
    struct strip<std::tuple<T...>>
    {
        using type = vct<T...>;
    };

    template<typename...T>
    void Something(T&&...args) { }

    int main3()
    {
        using U = std::tuple<int, char, std::string>;
        using X = vct<int, char, std::string>;
        using Y = strip<U>::type; // should be same as X

        return 0;
    }
}

export namespace TupleIteration
{
    // From https://www.cppstories.com/2022/tuple-iteration-apply/
    template <typename TupleT, typename Fn>
    void for_each_tuple2(TupleT&& tp, Fn&& fn)
    {
        std::apply(
            [&fn]<typename ...T>(T&& ...args) // C++20 template lambda
        {
            (fn(std::forward<T>(args)), ...);
        },
            std::forward<TupleT>(tp)
        );
    }

    void Lop(int i, int a) {}

    int main4()
    {
        for_each_tuple2(
            std::make_tuple(1, 2),
            [](auto&& x)
            {
                x *= 2;
            }
        );
        return 0;
    }
}

export namespace Requires
{
    template<int X>
    struct M
    {
        void A() requires (X == 1) { }
        void A() requires (X == 2) { }
    };

    int main5()
    {
        M<1> m1; m1.A();
        M<2> m2; m2.A();
        return 0;
    }
}

export namespace Chained
{
    // https://codereview.stackexchange.com/questions/274003/functor-chaining-function-for-c20
    // https://github.com/user1095108/generic/blob/master/invoke.hpp
    // https://github.com/user1095108/generic/blob/master/invoke.cpp
    auto const inc = [](const int i) noexcept
        {
            return i + 1;
        };

    auto const first = []() noexcept
        {
            return 0;
        };

    auto ChainApply(auto&& a, auto&& f, auto&& ...fs)
    {
        if constexpr (sizeof...(fs))
            return ChainApply(std::apply(f, std::forward_as_tuple(a)), std::forward<decltype(fs)>(fs)...);
        else
            return std::apply(f, std::forward_as_tuple(a));
    }

    void Run()
    {
        /*std::cout << std::format(
            "{}\n",
            ChainApply(0, inc, inc, inc)
        );*/
        std::cout << std::format(
            "{}\n",
            ChainApply(0, inc, inc, inc)
        );
    }
}

export namespace LoopOverElements
{
    void Do(auto&& x)
    {
        std::cout << typeid(x).name() << std::endl;
    }

    void Run()
    {
        std::tuple t{ 1, false };
        std::apply(
            []<typename...T>(T&&...args)
        {
            (Do(std::forward<T>(args)), ...);
        },
            std::forward<decltype(t)>(t)
        );
    }
}

export namespace LoopOverTypes
{
    struct A
    {
        int operator()()
        {
            return 1;
        }
    };
    struct B
    {
        int operator()(int i)
        {
            return i + 1;
        }
    };

    template<typename T>
    auto Do()
    {
        std::cout << typeid(T).name() << std::endl;
    }

    void Run()
    {
        std::tuple t{ 1, false };
        std::apply(
            []<typename...T>(T&&...args)
        {
            (Do<decltype(args)>(), ...);
        },
            std::forward<decltype(t)>(t)
        );
    }
}

export namespace LoopOverLambdas
{
    auto Do(auto&& lambda)
    {
        std::invoke(lambda);
    }

    void Run()
    {
        std::tuple t{ [] { std::cout << "Hello, one!\n"; }, [] { std::cout << "Hello, two!\n"; } };
        std::apply(
            []<typename...T>(T&&...args)
        {
            (Do(args), ...);
        },
            std::forward<decltype(t)>(t)
        );
    }
}

export namespace GetTupleElement
{
    // https://stackoverflow.com/a/58674921/7448661
    // Calls your func with tuple element.
    template <class Func, class Tuple, size_t N = 0>
    void runtime_get(Func func, Tuple& tup, size_t idx) 
    {
        if (N == idx) 
        {
            if constexpr (std::is_invocable_v<Func, decltype(std::get<N>(tup))>)
            {
                std::invoke(func, std::get<N>(tup));
            }
            return;
        }

        if constexpr (N + 1 < std::tuple_size_v<Tuple>) 
        {
            return runtime_get<Func, Tuple, N + 1>(func, tup, idx);
        }
    }

    template <class Func, class Tuple, size_t N = 0>
    auto runtime_get2(Func func, Tuple& tup, size_t idx)
    {
        if (N == idx)
        {
            if constexpr (std::is_invocable_v<Func, decltype(std::get<N>(tup))>)
                return std::invoke(func, std::get<N>(tup));
            else
                throw std::runtime_error("");
        }

        if constexpr (N + 1 < std::tuple_size_v<Tuple>)
            return runtime_get2<Func, Tuple, N + 1>(func, tup, idx);
    }

    // Calls your func with a pointer to the type.
    // Uses a pointer so the element is not initialized.
    template <class Tuple, class Func, size_t N = 0>
    void runtime_tuple_element(Func func, size_t idx) 
    {
        if (N == idx) 
        {
            std::tuple_element_t<N, Tuple>* ptr = nullptr;
            std::invoke(func, ptr);
            return;
        }

        if constexpr (N + 1 < std::tuple_size_v<Tuple>) 
        {
            return runtime_tuple_element<Tuple, Func, N + 1>(func, idx);
        }
    }

    std::string Something(const std::string& c)
    {
        std::string f = c;
        return f;
    }

    void Run()
    {
        std::tuple t{ 1, std::string{} };
        runtime_get(Something, t, 1);
        std::string s = runtime_get2(Something, t, 1);
    }
}

export namespace InvokeAtImpl
{
    // https://stackoverflow.com/a/71148253/7448661
    template<typename TPred, typename ...Ts, size_t ...Is>
    void invoke_at_impl(std::tuple<Ts...>& tpl, std::index_sequence<Is...>, size_t idx, TPred pred)
    {
        ((void)(Is == idx && (pred(std::get<Is>(tpl)), true)), ...);
        // for example: std::tuple<int, float, bool> `transformations` (idx == 1):
        //
        // Is... expansion    -> ((void)(0 == idx && (pred(std::get<0>(tpl)), true)), (void)(1 == idx && (pred(std::get<1>(tpl)), true)), (void)(2 == idx && (pred(std::get<2>(tpl)), true)));
        //                    -> ((void)(false && (pred(std::get<0>(tpl)), true)), (void)(true && (pred(std::get<1>(tpl)), true)), (void)(false && (pred(std::get<2>(tpl)), true)));
        // '&&' short-circuit -> ((void)(false), (void)(true && (pred(std::get<1>(tpl)), true)), (void)(false), true)));
        //
        // i.e. pred(std::get<1>(tpl) will be executed ONLY for idx == 1
    }

    template<typename TPred, typename ...Ts>
    void invoke_at(std::tuple<Ts...>& tpl, size_t idx, TPred pred)
    {
        invoke_at_impl(tpl, std::make_index_sequence<sizeof...(Ts)>{}, idx, pred);
    }

    void Something(const std::string& c)
    {
        std::string f = c;
    }

    void Run()
    {
        std::tuple t{ 1, std::string{} };
        //invoke_at(t, 1, Something);
    }
}
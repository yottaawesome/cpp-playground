export module tupletoparameterpack;
import std;
import std.compat;

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

export namespace DelayedDispatch2
{
    template<auto F, typename...T>
    struct X
    {
        std::tuple<T...> T{};
        void Run()
        {
            std::apply(F, T);
        }
    };

    void Func(int x) { std::println("{}", x); };

    void Run()
    {
        X<Func, int> m = { std::make_tuple(1) };
        m.Run();
    }
}

export namespace BlabBlah
{
    void BlahBlah(auto&&...args) 
    {
        (std::println("{}", args), ...);
    }

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

    int Run()
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
    void for_each_tuple(TupleT&& tp, Fn&& fn)
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
        for_each_tuple(
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
    auto const inc = [](const int i) noexcept { return i + 1; };
    auto const first = []() noexcept { return 0; };

    // Chain-invokes a variadic sequence of functions with each output being returned to the next function
    auto ChainApply(auto&& arg, auto&& func, auto&&...funcs)
    {
        if constexpr (sizeof...(funcs))
            return ChainApply(std::apply(func, std::forward_as_tuple(arg)), std::forward<decltype(funcs)>(funcs)...);
        else
            return std::apply(func, std::forward_as_tuple(arg));
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
        std::print("{}", typeid(x).name());
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
        std::println("{}", typeid(T).name());
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
        std::tuple t{ [] { std::println("Hello, one!"); }, [] { std::println("Hello, two!"); } };
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
    void RuntimeSet(Func func, Tuple& tup, size_t idx) 
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
            return RuntimeSet<Func, Tuple, N + 1>(func, tup, idx);
        }
    }

    template <class RType, class Func, class Tuple, size_t N = 0>
    RType runtime_get2(Func func, Tuple& tup, size_t idx)
    {
        if (N == idx)
        {
            if constexpr (std::is_invocable_v<Func, decltype(std::get<N>(tup))>)
                return std::invoke(func, std::get<N>(tup));
            return {};
        }

        if constexpr (N + 1 < std::tuple_size_v<Tuple>)
            return runtime_get2<RType, Func, Tuple, N + 1>(func, tup, idx);

        return {};
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

    int Something2(const int& c)
    {
        return c;
    }

    void Run()
    {
        std::tuple t{ 1, std::string{} };
        RuntimeSet(Something, t, 1);
        std::string s = runtime_get2<std::string>(Something, t, 1);
        int s2 = runtime_get2<int>(Something2, t, 0);
    }
}

export namespace InvokeAtImpl
{
    // https://stackoverflow.com/a/71148253/7448661
    // Sadly, doesn't work
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

    bool Something(int&& c)
    {
        return true;
        //std::string f = c;
    }

    template<typename T>
    bool Something2(T&& c)
    {
        return true;
        //std::string f = c;
    }

    void Run()
    {
        std::tuple t{ 1, std::string{} };
        invoke_at(t, 1, [](auto&& s) {});
    }
}

export namespace InvokeAtImpl2
{
    // https://stackoverflow.com/a/65925007/7448661
    template <typename T>
    inline constexpr size_t tuple_size_v = std::tuple_size<T>::value;

    template <typename T, typename F, std::size_t... I>
    constexpr void visit_impl(T& tup, const size_t idx, F fun, std::index_sequence<I...>)
    {
        //assert(idx < tuple_size_v<T>);
        ((I == idx ? fun(std::get<I>(tup)) : void()), ...);
    }

    template <typename F, typename... Ts, typename Indices = std::make_index_sequence<sizeof...(Ts)>>
    constexpr void visit_at(std::tuple<Ts...>& tup, const size_t idx, F fun)
    {
        visit_impl(tup, idx, fun, Indices{});
    }

    template <typename F, typename... Ts, typename Indices = std::make_index_sequence<sizeof...(Ts)>>
    constexpr void visit_at(const std::tuple<Ts...>& tup, const size_t idx, F fun)
    {
        visit_impl(tup, idx, fun, Indices{});
    }

    void Run()
    {
        auto tuple = std::tuple{ 1, 2.5, 3, 'Z', std::string{} };
        // print it to cout
        for (size_t i = 0; i < tuple_size_v<decltype(tuple)>; ++i) 
        {
            visit_at(tuple, i, [](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                std::cout << *typeid(T).name() << arg << ' ';
            });
        }

        visit_at(
            tuple,
            4,
            [](auto&& arg) { }
        );
    }
}

export namespace TupleFunc
{
    // Adapted from https://www.foonathan.net/2020/05/fold-tricks/
    template <class Tuple, class F>
    constexpr decltype(auto) for_each(Tuple&& tuple, F&& f)
    {
        return[]<std::size_t... I>(Tuple && tuple, F && f, std::index_sequence<I...>)
        {
            (f(std::get<I>(tuple)), ...);
            return f;
        }(
            std::forward<Tuple>(tuple), std::forward<F>(f),
            std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{}
        );
    }

    template<typename F, typename A>
    auto run_if_possible(F&& f, A&& a)
    {
        if constexpr (std::is_invocable_v<F, A>)
            return std::invoke(f, a);
    }

    template <class Tuple, class F>
    constexpr decltype(auto) conditional_for_each(Tuple&& tuple, F&& f)
    {
        return[]<std::size_t... I>(Tuple && tuple, F && f, std::index_sequence<I...>)
        {
            (run_if_possible(f, std::get<I>(tuple)), ...);
            return f;
        }(
            std::forward<Tuple>(tuple),
            std::forward<F>(f),
            std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{}
        );
    }

    template <class Tuple, class F>
    constexpr decltype(auto) do_at(Tuple&& tuple, F&& f, const size_t index)
    {
        return[index]<std::size_t... I>(Tuple && tuple, F && f, std::index_sequence<I...>)
        {
            ((I == index ? run_if_possible(f, std::get<I>(tuple)) : void()), ...);
            return f;
        }(
            std::forward<Tuple>(tuple),
            std::forward<F>(f),
            std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{}
        );
    }

    template <class TTuple, class TPredFn>
    constexpr decltype(auto) index_of(TTuple&& tuple, TPredFn&& predicate)
    {
        size_t index = (std::numeric_limits<size_t>::max)(); // workaround for Microsoft's shitty MAX macro
        [&index]<std::size_t... I>(TTuple&& tuple, TPredFn&& predicate, std::index_sequence<I...>)
        {
            ((predicate(std::get<I>(tuple)) ? (index = I, true) : (index, false)) or ...);
        }(
            std::forward<TTuple>(tuple),
            std::forward<TPredFn>(predicate),
            std::make_index_sequence<std::tuple_size<std::remove_reference_t<TTuple>>::value>{}
        );
        return index;
    }

    void Run()
    {
        std::tuple t{ 1, std::string{"a"}, 2 };
        conditional_for_each(t, [](const std::string& s) { std::println("Hello1"); });
        do_at(t, [](const std::string& s) { std::println("Hello2"); }, 1);
        std::println(
            "{}",
            index_of(t, [](auto&& v) { return std::same_as<std::remove_cvref_t<decltype(v)>, std::string>; })
        );
    }
}

export namespace TupleToVector
{
    // Adapted from https://stackoverflow.com/a/42495119/7448661
    template <class Tuple, class T = std::decay_t<std::tuple_element_t<0, std::decay_t<Tuple>>>>
    std::vector<T> to_vector(Tuple&& tuple)
    {
        return std::apply(
            [](auto&&... elems)
            {
                return std::vector<T>{std::forward<decltype(elems)>(elems)...};
            }, 
            std::forward<Tuple>(tuple)
        );
    }

    // Adapted from https://stackoverflow.com/a/42495119/7448661
    template <class Tuple, class T = std::decay_t<std::tuple_element_t<0, std::decay_t<Tuple>>>>
    std::vector<T> to_vector_no_copies(Tuple&& tuple)
    {
        return std::apply(
            []<typename...T>(T&&... elems) 
            {
                std::vector<T> result;
                result.reserve(sizeof...(elems));
                (result.push_back(std::forward<T>(elems)), ...);
                return result;
            }, 
            std::forward<Tuple>(tuple)
        );
    }
}

export namespace TupleConcatenation
{
    void Run()
    {
        std::tuple t1{ 1,2 };
        std::tuple t2{ 3,4 };
        std::tuple t3 = std::tuple_cat(t1, t2);
    }
}

export namespace Waiting
{
    using native_handle = void*; // Basically a Windows HANDLE

    // Functor type to execute when the associated handle is signaled
    struct WaitFunctionOne
    {
        native_handle handle = nullptr; // would point to a valid handle
        void operator()() { /* implementation here; */ }
    };

    // Functor type to execute when the associated handle is signaled
    struct WaitFunctionTwo
    {
        native_handle handle = nullptr; // would point to a valid handle
        void operator()() { /* implementation here; */ }
    };

    // Helper template to convert a tuple to vector of native_handles
    template <typename TReturn, typename TTuple, typename TTransform>
    std::vector<TReturn> ToVector(TTuple&& tuple, TTransform&& transform)
    {
        return std::apply(
            [&transform](auto&&... elems) -> std::vector<TReturn>
            {
                return {transform(elems)...};
            },
            std::forward<TTuple>(tuple)
        );
    }

    template<typename TTuple>
    void Wait(TTuple&& tuple)
    {
        std::vector<native_handle> waits = ToVector<native_handle>(
            tuple, 
            [](auto&& c) { return c.handle; }
        );

        // Do some kind of wait that gives us an index of the fired event
        unsigned long index = 1;

        // Invokes the functor in the tuple at the fired index
        []<std::size_t... I>(TTuple& tuple, size_t idx, std::index_sequence<I...>)
        {
            (((idx == I) ? (std::get<I>(tuple)(), false) : (void(), true)) and ...);
            // alternative (((idx == I) ? std::get<I>(tuple)() : void()), ...);
            //(([](size_t s) { std::println("Index is {}", s); }(I)), ...);
        }(
            tuple,
            index,
            std::make_index_sequence<std::tuple_size<std::remove_cvref_t<TTuple>>::value>{}
        );
    }

    void Run()
    {
        std::tuple waitables{
            WaitFunctionOne{},
            WaitFunctionTwo{}
        };

        Wait(waitables);
    }
}

export namespace PrettyPrint
{
    void pretty_print(
	    const std::string_view message,
	    const std::source_location& location = std::source_location::current(),
	    const std::stacktrace& trace = std::stacktrace::current()
    )
    {
	    constexpr std::string_view entryFmt = "  + {} {:>{}} {}:{}\n";
        // Determine necessary padding
        constexpr auto minPadding = 2;
        size_t fixedSize = minPadding;
        std::for_each(
            trace.begin(), trace.end(),
            [&fixedSize](const std::stacktrace_entry& entry)
            {
                std::string description = entry.description();
                if (description.size() > fixedSize - minPadding)
                    fixedSize = description.size() + minPadding;
            }
        );
        // Format stacktrace
	    std::string traceMsg;
        for (const std::stacktrace_entry& entry : trace)
        {
            std::string description = entry.description();
            if (description.contains("invoke_main"))
                break;
            size_t size = fixedSize - description.size();
            traceMsg += std::format(entryFmt, description, "@", size, entry.source_file(), entry.source_line());
        }

	    constexpr std::string_view messageFmt = 
R"(Error: {}
  in function: {}
  of file:     {}:{}
Stacktrace:
{})";

	    std::println(messageFmt, message, location.function_name(), location.file_name(), location.line(), traceMsg);
    }

    void Run()
    {
        pretty_print("something broke");
    }
}

export namespace Pairing
{
    template<int I>
    struct M
    {
	    constexpr int Get() const noexcept { return I; }
    };

    template<typename T>
    struct IsPair : std::false_type {};

    template<typename T0, typename T1>
    struct IsPair<std::pair<T0, T1>> : std::true_type {};

    template<typename T>
    constexpr bool IsPairV = IsPair<T>::value;

    template<typename T>
    concept Pair = IsPairV<std::remove_cvref_t<T>>;

    void Run()
    {
	    M<1> m1;
	    M<2> m2;

        []<typename...T>(T&&...values)
        {
            // https://www.fluentcpp.com/2021/12/13/the-evolutions-of-lambdas-in-c14-c17-and-c20/
            [...S = values]() 
            {
                (std::println("{}", S), ...);
            }();
            [...S = std::forward<decltype(values)>(values)]() 
            {
                (std::println("{}", S), ...);
            }();
        }(1, 2, 3);

        []<std::convertible_to<std::string_view> T>(T&& value){ }("a");
        bool b = [](std::convertible_to<std::string_view> auto&&...value)
        {
            bool b = false;
            return ((
                std::println("{}", value), 
                std::println("{}", value),
                b = 1 == 1
            ), ...);
        }("a", std::string{"a"});

        std::tuple t{ 1, 2 };
        []<typename...Ts, size_t...Is>(std::tuple<Ts...>&& tuple, std::index_sequence<Is...> seq)
        {
            ((std::get<Is>(tuple) == 0 ? true : false), ...);
        }(std::forward<decltype(t)>(t), std::make_index_sequence<std::tuple_size_v<decltype(t)>>{});

        []<typename T, typename S>(std::pair<T, S>&& pairs){ }(std::pair{1,2});

        [](Pair auto&&...pairs)
	    {
		    ([](Pair auto&& p, const int minimum)
		    {
			    const int value = std::get<1>(p).Get();
                if (value < minimum)
                    std::println("The value for {} is wrong {}", value, std::get<0>(p));
            }(pairs, 2), ...);
	    }(
            std::pair{ "a", m1 },
            std::pair{ "b", m2 }
	    );   
    }
}

export namespace Structs
{
    std::default_random_engine engn;
    std::uniform_int_distribution<int> dist(1, 50);

    struct A
    {
	    int M = 
		    []{
			    PrettyPrint::pretty_print("Lambda dummy error");
			    return dist(engn);
		    }();

	    int N = 
		    [](int M) 
		    {
			    return M;
		    }(M);
    };

    void Run()
    {
        if constexpr (std::is_standard_layout_v<A>)
            std::println("Yes");
        else
            std::println("No");
        // https://developercommunity.visualstudio.com/t/10452770
        //std::println("{}", std::is_standard_layout_v<A>);
    }
}

export namespace FilterVariadic
{   
    // https://stackoverflow.com/questions/60006076/filter-types-in-a-parameter-pack
    // https://ideone.com/MzUtL6
    template <
        template <class> class,
        template <class...> class,
        class...
    >
    struct filter;
    template <template <class> class Pred, template <class...> class Variadic>
    struct filter<Pred, Variadic>
    {
        using type = Variadic<>;
    };

    template<
        template <class> class Pred,
        template <class...> class Variadic,
        class T, class... Ts
    >
    struct filter<Pred, Variadic, T, Ts...>
    {
        template <class, class> 
        struct Cons;
        
        template <class Head, class... Tail>
        struct Cons<Head, Variadic<Tail...> >
        {
            using type = Variadic<Head, Tail...>;
        };

        using type = typename std::conditional<
            Pred<T>::value,
            typename Cons<T, typename filter<Pred, Variadic, Ts...>::type>::type,
            typename filter<Pred, Variadic, Ts...>::type
        >::type;
    };

    void Run() 
    {
        static_assert(
            std::is_same<
                filter<std::is_integral, std::tuple, int, float, long>::type, 
                std::tuple<int, long>
            >::value
        );
    }
}

export namespace TupleCats
{
    bool Predicate(auto& element)
    {
        return element.IsValue;
    }

    template<typename T>
    struct S
    {
        bool IsValue = false;
    };

    using A = S<struct I>;
    using B = S<struct II>;
    using C = S<struct III>;

    template<bool And = true>
    bool Something(auto&& tuple, auto&& pred)
    {
        using T = std::remove_cvref_t<decltype(tuple)>;
        return []<size_t...Is>(auto&& tuple, auto&& pred, std::index_sequence<Is...>)
        {
            if constexpr (And)
                return ((pred(std::get<Is>(tuple)) ? (true) : (false)) and ...);
            else
                return ((pred(std::get<Is>(tuple)) ? (true) : (false)) or ...);
        }(
            std::forward<decltype(tuple)>(tuple), 
            std::forward<decltype(pred)>(pred), 
            std::make_index_sequence<std::tuple_size_v<T>>{}
        );
    }

    struct F 
    { 
        //friend auto operator<=>(const F&, const F&) = default;
        auto operator<=>(const F&) const = default;
    };

    template<typename T, typename...R>
    concept L = (std::same_as<T, R> or ...);

    void Run()
    {
        static_assert(L<int, int, float>);
        std::tuple t{ A{true}, B{false} };
        F a;
        F b;
        std::println("{} {}", a == b, a < b);
        Something(std::forward<decltype(t)>(t), [](auto&& v) { return v.IsValue; });
    }
}
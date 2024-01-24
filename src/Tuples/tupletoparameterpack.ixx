module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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

        visit_at(tuple, 4, [](auto&& arg) {

            });
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

export namespace WaitingB
{
    template<typename T>
    struct is_array : std::false_type {};

    template<typename T, size_t N>
    struct is_array<std::array<T, N>> : std::true_type {};

    template<typename T, size_t N>
    constexpr bool is_array_v = is_array<T, N>;

    template<typename T>
    concept array_like = is_array<std::remove_cvref_t<T>>::value;

    constexpr auto a = std::array<int, 3>{1, 2, 3};
    static_assert(array_like<decltype(a)>);

    template<class T>
    struct is_duration : std::false_type {};

    template<class Rep, class Period>
    struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type {};

    template<typename T>
    concept duration = is_duration<T>::value;

    template<typename T>
    concept waitable =
        not std::is_null_pointer_v<T>
        and (
            std::same_as<std::remove_cvref_t<T>, void*>
            //or array_like<T>
            or requires(const T t)
            {
                {t.get_handle()} noexcept -> std::same_as<void*>;
            }
        );

    

    template<typename TDummy>
    class handle_t final
    {
        public:
            ~handle_t() { close(); }

            handle_t() noexcept = default;

            handle_t(const handle_t&) = delete;
            handle_t operator=(const handle_t&) = delete;
            handle_t(handle_t&& other) { move(other); }
            handle_t operator=(handle_t&& other) { move(other); }

        public:
            void* get_handle() const noexcept { return m_handle; }

        private:
            void close() noexcept
            {
                if (not m_handle)
                    return;
                CloseHandle(m_handle);
                m_handle = nullptr;
            }

            void move(handle_t& other) noexcept
            {
                close();
                m_handle = other.m_handle;
                other.m_handle = nullptr;
            }

        private:
            void* m_handle =
                []() noexcept 
                {
                    void* handle = CreateEvent(nullptr, true, false, nullptr);
                    if (not handle)
                        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
                    return handle;
                }();
    };

    using handle_a = handle_t<struct handle_a_d>;
    using handle_b = handle_t<struct handle_b_d>;
    static_assert(waitable<handle_b> and waitable<handle_a>);

    template<typename T>
    struct false_type : std::false_type {};

    template<waitable T>
    constexpr auto to_handle(T&& item)
    {
        using TU = std::remove_cvref_t<T>;
        if constexpr (std::same_as<TU, void*>)
            return item;
        else if constexpr (array_like<TU>)
            return item;
        else
            return item.get_handle();
    }

    auto create_array(auto&&...args)
    {
        /*if constexpr (sizeof...(args) ==  1)
        {
            static_assert()
        }*/
    }

    unsigned wait_on(
        const bool wait_for_all,
        const bool is_interruptible_wait,
        const duration auto timespan,
        waitable auto&&... waitables
    )
    {
        static_assert(sizeof...(waitables) > 0, "Must pass at least one waitable.");
        static_assert(sizeof...(waitables) <= MAXIMUM_WAIT_OBJECTS, "Cannot wait on more than MAXIMUM_WAIT_OBJECTS objects.");

        const auto waitable_array = std::array<void*, sizeof...(waitables)>{ to_handle(waitables)... };
        if (std::any_of(waitable_array.begin(), waitable_array.end(), [](auto&& h) { return h == nullptr; }))
            throw std::runtime_error("Unexpected nullptr for handle");

        using std::chrono::duration_cast, std::chrono::milliseconds;
        const unsigned result = WaitForMultipleObjectsEx(
            static_cast<unsigned>(waitable_array.size()),
            waitable_array.data(),
            wait_for_all,
            static_cast<unsigned>(duration_cast<milliseconds>(timespan).count()),
            is_interruptible_wait
        );
        if (result == WAIT_ABANDONED)
            throw std::runtime_error("The wait was unexpectedly abandoned");
        if (result == WAIT_FAILED)
            throw std::runtime_error("The wait unexpectedly failed");

        return result;
    }

    template<typename T, size_t N>
    auto array_to_tuple(const std::array<T, N>& arr)
    {
        // std::tuple_size_v<std::remove_cvref_t<decltype(arr)>> works too
        return []<size_t...Is>(const std::array<T, N>& arr, std::index_sequence<Is...>)
        {
            return std::tuple{ arr[Is]... };
        }(arr, std::make_index_sequence<N>{});
    }

    unsigned wait_on(
        const bool wait_for_all,
        const bool is_interruptible_wait,
        const duration auto timespan,
        array_like auto&& waitables
    )
    {
        return std::apply(
            [wait_for_all, is_interruptible_wait, timespan]<typename...T>(T&&...n)
            {
                return wait_on(wait_for_all, is_interruptible_wait, timespan, std::forward<T>(n)...); 
            },
            waitables
        );
    }

    void Random()
    {
        std::index_sequence<3> arr[3];

        auto x = []<size_t...Is>(std::index_sequence<Is...>)
        {
            return (
                []<size_t I = Is>()
                {
                    if constexpr (I == 1)
                    {

                    }
                }(), ...
            );
        };
        x(std::make_index_sequence<3>{});
    }

    void Run()
    {
        handle_a a{};
        handle_b b{};
        std::jthread t(
            [](handle_a& a) 
            {
                std::this_thread::sleep_for(std::chrono::seconds{ 1 });
                std::println("Event signaled succesfully");
                SetEvent(a.get_handle());
            },
            std::ref(a)
        );

        unsigned result = wait_on(false, false, std::chrono::seconds{ 5 }, a, b);
        unsigned result2 = wait_on(false, false, std::chrono::seconds{ 5 }, std::array<void*, 2>{nullptr, nullptr});
        if (result == WAIT_TIMEOUT)
            std::println("Timed out");
        else if (result == 0)
            std::println("Wait one succeeded");
    }
}

export namespace AnotherWait
{
    template<class T>
    struct is_duration : std::false_type {};

    template<class Rep, class Period>
    struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type {};

    template<typename T>
    concept duration = is_duration<T>::value;

    template<typename T>
    concept waitable =
        not std::is_null_pointer_v<T>
        and (
            std::same_as<std::remove_cvref_t<T>, void*>
            or requires(const T t)
            {
                {t.get_handle()} noexcept -> std::same_as<void*>;
            }
    );

    template<typename T>
    struct is_vector : std::false_type {};

    template<typename T>
    struct is_vector<std::vector<T>> : std::true_type {};

    template<typename T>
    constexpr bool is_vector_v = is_vector<std::vector<T>>::value;

    template<typename T>
    struct is_array : std::false_type {};

    template<typename T, size_t N>
    struct is_array<std::array<T, N>> : std::true_type {};

    template<typename T, size_t N>
    constexpr bool is_array_v = is_array<T, N>;

    template<typename T>
    concept arraylike = is_array_v<T>;

    template<waitable T>
    constexpr auto to_handle(T&& item)
    {
        using TU = std::remove_cvref_t<T>;
        if constexpr (std::same_as<TU, void*>)
            return item;
        else
            return item.get_handle();
    }

    unsigned internal_wait_on(
        const bool wait_for_all,
        const bool is_interruptible_wait,
        const duration auto timespan,
        const void* waitables,
        const size_t count
    )
    {
        const unsigned result = WaitForMultipleObjectsEx(
            static_cast<unsigned>(count),
            static_cast<const HANDLE*>(waitables),
            wait_for_all,
            static_cast<unsigned>(std::chrono::duration_cast<std::chrono::milliseconds>(timespan).count()),
            is_interruptible_wait
        );
        if (result == WAIT_ABANDONED)
            throw std::runtime_error("The wait was unexpectedly abandoned");
        if (result == WAIT_FAILED)
            throw std::runtime_error("The wait unexpectedly failed");

        return result;
    }

    template<typename T>
    constexpr bool not_array(T&& t)
    {
        static_assert(not arraylike<std::remove_cvref_t<T>>);
    }

    void not_array_one(auto&&...waitables)
    {
        if constexpr (sizeof...(waitables) > 0)
        {
            []<size_t...Is, typename TTuple>(std::index_sequence<Is...>, TTuple&& args)
            {
                (not_array(std::get<Is>(args)), ...);
            }(
                std::make_index_sequence<sizeof...(waitables)>{},
                std::forward_as_tuple(waitables...)
            );
        }
    }

    unsigned wait_on(
        const bool wait_for_all,
        const bool is_interruptible_wait,
        const duration auto timespan,
        waitable auto&&... waitables
    )
    {
        static_assert(sizeof...(waitables) > 0, "Must pass at least one waitable.");
        static_assert(sizeof...(waitables) <= MAXIMUM_WAIT_OBJECTS, "Cannot wait on more than MAXIMUM_WAIT_OBJECTS objects.");

        const auto waitable_array = std::array<void*, sizeof...(waitables)>{ to_handle(waitables)... };
        if (std::any_of(waitable_array.begin(), waitable_array.end(), [](auto&& h) { return h == nullptr; }))
            throw std::runtime_error("Unexpected nullptr for handle");

        return internal_wait_on(
            wait_for_all,
            is_interruptible_wait,
            timespan,
            waitable_array.data(),
            waitable_array.size()
        );
    }

    template<size_t N>
    unsigned wait_on(
        const bool wait_for_all,
        const bool is_interruptible_wait,
        const duration auto timespan,
        const std::array<void*, N>& waitables
    )
    {
        return internal_wait_on(wait_for_all, is_interruptible_wait, timespan, waitables.data(), N);
    }

    unsigned wait_on(
        const bool wait_for_all,
        const bool is_interruptible_wait,
        const duration auto timespan,
        const std::vector<void*>& waitables
    )
    {
        return internal_wait_on(wait_for_all, is_interruptible_wait, timespan, waitables.data(), waitables.size());
    }

    void Run()
    {
        void* a = CreateEvent(nullptr, true, false, nullptr);
        void* b = CreateEvent(nullptr, true, false, nullptr);
        wait_on(false, false, std::chrono::seconds{ 2 }, std::vector{ a, b });
        wait_on(false, false, std::chrono::seconds{ 2 }, std::array{ a, b });
        wait_on(false, false, std::chrono::seconds{ 2 }, a);
    }
}
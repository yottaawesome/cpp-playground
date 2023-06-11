export module tupletoparameterpack;
import std;

// See https://stackoverflow.com/questions/36612596/tuple-to-parameter-pack
// And https://www.cppstories.com/2022/tuple-iteration-apply/
export namespace ParamPacks
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

    void BlahBlah(auto&&...args)
    {

    }

    template<typename...T>
    void BlahBlahCaller(T&&...t)
    {
        std::apply(BlahBlah<T...>, std::make_tuple(std::forward<T>(t)...));
    }

    int main2()
    {
        BlahBlahCaller(1, 2);
        std::apply(BlahBlah<int, int>, std::make_tuple(1,2));

        return 0;
    }

    // And https://stackoverflow.com/questions/15418841/how-do-i-strip-a-tuple-back-into-a-variadic-template-list-of-types
    template<typename...X>
    struct vct
    {

    };

    template<typename>
    struct strip;

    template<typename ...T>
    struct strip<std::tuple<T...>>
    {
        using type = vct<T...>;
    };

    template<typename...T>
    void Something(T&&...args)
    {

    }

    int main3()
    {
        using U = std::tuple<int, char, std::string>;
        using X = vct<int, char, std::string>;
        using Y = strip<U>::type; // should be same as X

        return 0;
    }

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
}
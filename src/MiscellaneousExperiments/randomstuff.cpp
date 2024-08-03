module randomstuff;
import std;
import std.compat;

// https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/
// https://www.youtube.com/watch?v=jXf--bazhJw
namespace DeducingThis
{
    struct M
    {
        void A(this auto&& m);
    };
    
    void M::A(this auto&& m)
    {

    }

    struct scheduler
    {
        bool submit(auto&& s) { return true; }
    };

    std::string get_message()
    {
        return "message";
    }

    // parameter lambdas
    int x = 6;
    void func(
        int i = [](int x) { return 3*x; }(x),
        int j = [](int x) { return x; }(x)
    )
    { }

    constexpr auto m = 
        [](std::convertible_to<std::string_view> auto&& value) requires std::convertible_to<decltype(value), std::string_view>
        {};

    template<typename TFn, typename...TArgs, typename TExpected = std::expected<std::invoke_result_t<TFn, TArgs...>, std::exception>>
    auto repeatable(int repetitions, TFn&& function, TArgs&&...args) -> TExpected
    try
    {
        return std::invoke(std::forward<TFn>(function), std::forward<TArgs>(args)...);
    }
    catch (const std::exception& ex)
    {
        if (repetitions > 1)
            return repeatable(--repetitions, function, std::forward<TArgs>(args)...);
        return std::unexpected(ex);
    }

    constexpr auto repeatable2 = 
        []<typename TFn, typename...TArgs, typename TExpected = std::expected<std::invoke_result_t<TFn, TArgs...>, std::exception>>(
            this auto&& self, 
            int repetitions, 
            TFn&& function, 
            TArgs&&...args
        ) -> TExpected
        {
            try
            {
                return std::invoke(std::forward<TFn>(function), std::forward<TArgs>(args)...);
            }
            catch (const std::exception& ex)
            {
                if (repetitions > 1)
                    return self(--repetitions, function, std::forward<TArgs>(args)...);
                return std::unexpected(ex);
            }
        };

    int sth(int x) { return x; }

    void Run()
    {
        repeatable(5, sth, 1);
        repeatable2(5, sth, 1);

        auto ff = []<typename T>(){};
        ff.operator()<int>();
        [](this auto&& self, int repetitions, auto&&...args) -> void
        {
            std::println("Repetition {}", repetitions);
            if (repetitions > 1)
                self(--repetitions, std::forward<decltype(args)>(args)...);
        }(5);

        scheduler sched{};
        auto callback = 
            [m = get_message(), &sched](this auto&& self) -> bool 
            {
                return sched.submit(std::forward_like<decltype(self)>(m));
            };
        callback(); // retry(callback)
        std::move(callback)(); // try-or-fail(rvalue)
    }
}

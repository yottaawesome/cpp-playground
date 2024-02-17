module randomstuff;
import std;
import std.compat;

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

    void Run()
    {
        auto ff = []<typename T>(){};
        ff.operator()<int>();

        [](this auto&& self, int repetitions) -> void
        {
            std::println("Repetition {}", repetitions);
            if (repetitions > 1)
                self(--repetitions);
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

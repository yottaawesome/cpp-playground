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

    void Run()
    {
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

module;

#include <cassert>

export module coroutines:tumble;
import std;
import std.compat;

// Adapted from https://en.cppreference.com/w/cpp/language/coroutines
export namespace Coroutines::Tumble
{
    struct tunable_coro
    {
        ~tunable_coro()
        {
            if (handle_)
                handle_.destroy();
        }

        // For simplicity, declare these 4 special functions as deleted:
        tunable_coro(tunable_coro const&) = delete;
        tunable_coro(tunable_coro&&) = delete;
        tunable_coro& operator=(tunable_coro const&) = delete;
        tunable_coro& operator=(tunable_coro&&) = delete;

        // An awaiter whose "readiness" is determined via constructor's parameter.
        struct tunable_awaiter
        {
            explicit(false) tunable_awaiter(bool ready) : ready_{ ready } {}
            // Three standard awaiter interface functions:
            bool await_ready() const noexcept { return ready_; }
            static void await_suspend(std::coroutine_handle<>) noexcept {}
            static void await_resume() noexcept {}
        private:
            bool ready_;
        };

        struct promise_type
        {
            using coro_handle = std::coroutine_handle<promise_type>;
            auto get_return_object() { return coro_handle::from_promise(*this); }
            static auto initial_suspend() { return std::suspend_always(); }
            static auto final_suspend() noexcept { return std::suspend_always(); }
            static void return_void() {}
            static void unhandled_exception() { std::terminate(); }
            // A user provided transforming function which returns the custom awaiter:
            auto await_transform(std::suspend_always) { return tunable_awaiter(!ready_); }
            void disable_suspension() { ready_ = false; }
        private:
            bool ready_{ true };
        };

        tunable_coro(promise_type::coro_handle h) : handle_(h) { assert(h); }

        void disable_suspension() const
        {
            if (handle_.done())
                return;
            handle_.promise().disable_suspension();
            handle_();
        }

        bool operator()()
        {
            if (!handle_.done())
                handle_();
            return !handle_.done();
        }

    private:
        promise_type::coro_handle handle_;
    };

    tunable_coro generate(int n)
    {
        for (int i{}; i != n; ++i)
        {
            std::cout << i << ' ';
            // The awaiter passed to co_await goes to promise_type::await_transform which
            // issues tunable_awaiter that initially causes suspension (returning back to
            // main at each iteration), but after a call to disable_suspension no suspension
            // happens and the loop runs to its end without returning to main().
            co_await std::suspend_always{};
        }
    }

    void Run()
    {
        auto coro = generate(8);
        coro(); // emits only one first element == 0
        for (int k{}; k < 4; ++k)
        {
            coro(); // emits 1 2 3 4, one per each iteration
            std::cout << ": ";
        }
        coro.disable_suspension();
        coro(); // emits the tail numbers 5 6 7 all at ones
    }
}
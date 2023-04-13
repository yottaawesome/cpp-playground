// Sample from https://en.cppreference.com/w/cpp/language/coroutines
module;

#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <format>

export module coroutines:threadswitch2;

export namespace Coroutines::ThreadSwitch2
{
    struct awaitable
    {
        std::jthread* p_out;
        bool await_ready() { return false; }
        void await_suspend(std::coroutine_handle<> h)
        {
            std::jthread& out = *p_out;
            if (out.joinable())
                throw std::runtime_error("Output jthread parameter not empty");
            std::cout << "This thread ID: " << std::this_thread::get_id() << '\n'; // this is OK
            out = std::jthread([h] { h.resume(); });
            std::cout
                << "out thread ID: "
                << out.get_id()
                << ", this thread: "
                << std::this_thread::get_id() << '\n';
        }
        void await_resume()
        {
            std::cout << "AwaitResume(): " << std::this_thread::get_id() << '\n';
        }
    };

    struct task
    {
        std::shared_ptr<int> value;
        task(std::shared_ptr<int> p) : value(p) { }

        int get()
        {
            return *value;
        }

        struct promise_type
        {
            std::shared_ptr<int> ptr = std::make_shared<int>();
            task get_return_object()
            {
                return { ptr };
            }
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
            //void return_void() {}
            void unhandled_exception() {}
            void return_value(int i)
            {
                *ptr = i;
            }
        };
    };

    auto switch_to_new_thread(std::jthread& out)
    {
        return awaitable{ &out };
    }

    task resuming_on_new_thread(std::jthread& out)
    {
        std::cout << "Coroutine started on thread: " << std::this_thread::get_id() << '\n';
        co_await switch_to_new_thread(out);
        // awaiter destroyed here
        std::cout << "Coroutine resumed on thread: " << std::this_thread::get_id() << '\n';

        co_return 1;
    }

    void RunSample()
    {
        std::cout
            << "Original thread " << std::this_thread::get_id() << '\n';
        std::jthread out;
        task x = resuming_on_new_thread(out);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout
            << x.get()
            << '\n'
            << "Original thread "
            << std::this_thread::get_id() << " exiting\n";
    }
}
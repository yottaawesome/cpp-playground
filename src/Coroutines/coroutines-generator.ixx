// Sample adapted from https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html
module;

#include <iostream>
#include <coroutine>

export module coroutines:generator;

export namespace Coroutines::Generator
{
    template<typename T>
    struct Generator 
    {
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;
        handle_type h_;

        struct promise_type 
        {
            T value_;
            std::exception_ptr exception_;

            Generator get_return_object() 
            {
                return Generator(handle_type::from_promise(*this));
            }
            std::suspend_always initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void unhandled_exception() { exception_ = std::current_exception(); }
            template<std::convertible_to<T> From> // C++20 concept
            std::suspend_always yield_value(From&& from) 
            {
                value_ = std::forward<From>(from);
                return {};
            }
            void return_void() {}
        };


        Generator(handle_type h) : h_(h) {}
        ~Generator() { h_.destroy(); }

        explicit operator bool() 
        {
            fill();
            return !h_.done();
        }

        T operator()() 
        {
            fill();
            full_ = false;
            return std::move(h_.promise().value_);
        }

    private:
        bool full_ = false;

        void fill() 
        {
            if (!full_) 
            {
                h_();
                if (h_.promise().exception_)
                    std::rethrow_exception(h_.promise().exception_);
                full_ = true;
            }
        }
    };

    Generator<unsigned> counter6()
    {
        for (unsigned i = 0; i < 3;)
            co_yield i++;
    }

    void RunSample()
    {
        auto gen = counter6();
        while (gen)
            std::cout << "counter6: " << gen() << std::endl;
    }
}
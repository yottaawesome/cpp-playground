export module coroutines:basic;
import std;

// https://github.com/Atliac/Coroutine-Tutorial/blob/master/hello_coroutine/main.cpp
//template<>
//struct std::coroutine_traits<void>
//{
//    struct promise_type
//    {
//        void get_return_object() noexcept {}
//        auto initial_suspend() noexcept { return std::suspend_never{}; }
//        auto final_suspend() noexcept { return std::suspend_never{}; }
//        void return_void() noexcept {}
//        void unhandled_exception() noexcept {}
//    };
//};
//
//export namespace Simple2
//{
//    void hello_coroutine()
//    {
//        std::println("Hello coroutine");
//        co_return ;
//    }
//
//    void Run()
//    {
//        hello_coroutine();
//    }
//}

export namespace Coroutines::Simplest
{
    struct resumable
    {
        struct awaitable
        {
            awaitable(resumable& t) : m_task(t)
            {

            }

            ~awaitable()
            {
                std::println("~awaitable()");
            }

            bool await_ready()
            {
                std::println("await_ready()");
                return false;
            }

            void await_suspend(std::coroutine_handle<> h)
            {
                std::println("await_suspend()");
                //m_task.handle = h;
            }

            void await_resume()
            {
                std::println("await_resume()");
            }

            resumable& m_task;
        };

        awaitable operator co_await()  noexcept
        {
            return awaitable{ *this };
        }

        //std::coroutine_handle<> handle;
    };

    struct task
    {
        struct promise_type
        {
            ~promise_type()
            {
                std::println("~promise_type()");
            }

            task get_return_object()
            {
                std::println("get_return_object()");
                return task(std::coroutine_handle<promise_type>::from_promise(*this));
            }

            std::suspend_never initial_suspend()
            {
                std::println("initial_suspend()");
                return {};
            }

            std::suspend_never final_suspend() noexcept
            {
                std::println("final_suspend()");
                return {};
            }

            void return_void()
            {
            }

            /*void return_value(int t)
            {
                std::println("return_value()");
            }*/

            void unhandled_exception()
            {
                std::println("unhandled_exception()");
            }
        };

        ~task()
        {
            std::println("~task()");
        }

        task(std::coroutine_handle<promise_type> h) : m_h(h) {}

        void Get()
        {
            m_h.resume();
        }

        std::coroutine_handle<promise_type> m_h;
    };

    /*auto CreateAwaitable()
    {
        return awaitable{};
    }*/

    task MakeTask()
    {
        std::println("MakeTask()");
        co_await resumable();
        // awaiter destroyed here
        //co_return 1;
        std::println("Hello");
    }

    void Run()
    {
        auto x = MakeTask();
        x.Get();
    }
}

export namespace Coroutines::EvenMoreBasicAwaitables
{
    struct OverlappedOperation
    {
        struct Awaitable
        {
            Awaitable(OverlappedOperation& t) : m_task(t)
            {

            }

            ~Awaitable()
            {
                std::println("~awaitable()");
            }

            bool await_ready() // --> 2
            {
                std::println("await_ready()");
                return false;
            }

            void await_suspend(std::coroutine_handle<> h) // --> 3
            {
                std::println("await_suspend()");
                //m_task.handle = h;
            }

            void await_resume()
            {
                std::println("await_result()");
            }

            OverlappedOperation& m_task;
        };

        Awaitable operator co_await()  noexcept
        {
            return Awaitable{ *this };
        }
    };

    struct Task
    {
        struct promise_type
        {
            ~promise_type()
            {
                std::println("~promise_type()");
            }

            Task get_return_object()
            {
                return {};
            }

            std::suspend_never initial_suspend() // --> 1
            {
                std::println("initial_suspend()");
                return {};
            }

            std::suspend_never final_suspend() noexcept
            {
                std::println("final_suspend()");
                return {};
            }

            void return_void()
            {
            }

            /*void return_value(int t)
            {
                std::println("return_value()");
            }*/

            void unhandled_exception()
            {
                std::println("unhandled_exception()");
            }
        };

        ~Task()
        {
            std::println("~task()");
        }
    };

    Task DoTask()
    {
        co_await OverlappedOperation();
    }

    void Run()
    {
        std::println("Coroutines::EvenMoreBasicAwaitables");
        DoTask();
    }
}
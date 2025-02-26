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
        resumable r;
        std::println("MakeTask()");
        co_await r;
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

    template<typename T>
    struct OverlappedOperation
    {
        struct Awaitable
        {
            Awaitable() noexcept

            {
            }

            ~Awaitable()noexcept
            {
                std::println("~awaitable()");
            }

            bool await_ready()noexcept // --> 2
            {
                std::println("await_ready()");
                return false;
            }

            void await_suspend(std::coroutine_handle<T> h) noexcept// --> 3
            {
                h.promise().set();
                std::println("await_suspend()");
                //H = h;
                //m_task.handle = h;
            }

            void await_resume()noexcept
            {
                std::println("await_result()");
            }
        };

        Awaitable operator co_await()  noexcept
        {
            //return Awaitable{ *this };
            return Awaitable{ };
        }
    };

    struct Task
    {
        struct promise_type
        {
            std::shared_ptr<int> M = std::make_shared<int>(0);

            void set() { *M = 10; }

            ~promise_type()
            {
                std::println("promise_type::~promise_type()");
            }

            Task get_return_object()
            {
                return {std::coroutine_handle<promise_type>::from_promise(*this), M };
            }

            std::suspend_never initial_suspend() // --> 1
            {
                std::println("promise_type::initial_suspend()");
                return {  };
            }

            std::suspend_never final_suspend() noexcept
            {
                std::println("promise_type::final_suspend()");
                return {};
            }

            void return_void()
            {
            }

            void unhandled_exception()
            {
                std::println("promise_type::unhandled_exception()");
            }
        };

        std::coroutine_handle<promise_type> Handle;
        std::shared_ptr<int> M;

        Task(std::coroutine_handle<promise_type> h, std::shared_ptr<int> m)
            : Handle(h), M(m)
        {

        }

        ~Task()
        {
            std::println("~task()");
        }

        void Continue()
        {
            Handle.resume();
        }
    };

    Task DoTask()
    {
        co_await OverlappedOperation<Task::promise_type>();
        std::println("resumed");
    }

    void Run()
    {
        std::println("Coroutines::EvenMoreBasicAwaitables");
        auto x = DoTask();
        x.Continue();
        std::println("{}", *x.M);
    }
}

export namespace Overlapped
{
   
}
// See https://www.modernescpp.com/index.php/a-concise-introduction-to-coroutines-by-dian-lun-li/
// and https://www.modernescpp.com/index.php/coroutines-a-scheduler-for-tasks-by-dian-lun-li/
export module coroutines:taskscheduler;
import std;

namespace TaskScheduler
{
    // stackScheduler.cpp
    struct Task
    {
        struct promise_type
        {
            std::suspend_always initial_suspend() noexcept { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }

            Task get_return_object()
            {
                return std::coroutine_handle<promise_type>::from_promise(*this);
            }
            void return_void() {}
            void unhandled_exception() {}
        };

        Task(std::coroutine_handle<promise_type> handle) : handle{ handle } {}

        auto get_handle() { return handle; }

        std::coroutine_handle<promise_type> handle;
    };

    class Scheduler
    {
        std::stack<std::coroutine_handle<>> _tasks;

        public:
            void emplace(std::coroutine_handle<> task)
            {
                _tasks.push(task);
            }

            void schedule()
            {
                while (!_tasks.empty())
                {
                    auto task = _tasks.top();
                    _tasks.pop();
                    task.resume();

                    if (!task.done())
                    {
                        _tasks.push(task);
                    }
                    else
                    {
                        task.destroy();
                    }
                }
            }

            auto suspend()
            {
                return std::suspend_always{};
            }
    };


    Task TaskA(Scheduler& sch)
    {
        std::cout << "Hello from TaskA\n";
        co_await sch.suspend();
        std::cout << "Executing the TaskA\n";
        co_await sch.suspend();
        std::cout << "TaskA is finished\n";
    }

    Task TaskB(Scheduler& sch)
    {
        std::cout << "Hello from TaskB\n";
        co_await sch.suspend();
        std::cout << "Executing the TaskB\n";
        co_await sch.suspend();
        std::cout << "TaskB is finished\n";
    }

    void Run()
    {
        std::cout << '\n';

        Scheduler sch;

        sch.emplace(TaskA(sch).get_handle());
        sch.emplace(TaskB(sch).get_handle());

        std::cout << "Start scheduling...\n";

        sch.schedule();

        std::cout << '\n';
    }
}

namespace QueueScheduler
{
    // queueScheduler.cpp
    struct Task 
    {
        struct promise_type 
        {
            std::suspend_always initial_suspend() noexcept { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }

            Task get_return_object() 
            {
                return std::coroutine_handle<promise_type>::from_promise(*this);
            }
            void return_void() {}
            void unhandled_exception() {}
        };

        Task(std::coroutine_handle<promise_type> handle) : handle{ handle } {}

        auto get_handle() { return handle; }

        std::coroutine_handle<promise_type> handle;
    };

    class Scheduler
    {
        std::queue<std::coroutine_handle<>> _tasks;

        public:
            void emplace(std::coroutine_handle<> task) 
            {
                _tasks.push(task);
            }

            void schedule() 
            {
                while (!_tasks.empty()) 
                {
                    auto task = _tasks.front();
                    _tasks.pop();
                    task.resume();

                    if (!task.done()) 
                    {
                        _tasks.push(task);
                    }
                    else 
                    {
                        task.destroy();
                    }
                }
            }

            auto suspend() 
            {
                return std::suspend_always{};
            }
    };

    Task TaskA(Scheduler& sch) 
    {
        std::cout << "Hello from TaskA\n";
        co_await sch.suspend();
        std::cout << "Executing the TaskA\n";
        co_await sch.suspend();
        std::cout << "TaskA is finished\n";
    }

    Task TaskB(Scheduler& sch) 
    {
        std::cout << "Hello from TaskB\n";
        co_await sch.suspend();
        std::cout << "Executing the TaskB\n";
        co_await sch.suspend();
        std::cout << "TaskB is finished\n";
    }

    void Run() 
    {
        std::cout << '\n';

        Scheduler sch;

        sch.emplace(TaskA(sch).get_handle());
        sch.emplace(TaskB(sch).get_handle());

        std::cout << "Start scheduling...\n";

        sch.schedule();

        std::cout << '\n';
    }
}
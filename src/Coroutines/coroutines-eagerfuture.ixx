// Adapted from https://www.modernescpp.com/index.php/implementing-futures-with-coroutines
export module coroutines:eagerfuture;
import std;

// eagerFuture.cpp

export namespace Coroutines::EagerFuture
{
    template<typename T>
    struct MyFuture 
    {
        std::shared_ptr<T> value;                           // (3)
        MyFuture(std::shared_ptr<T> p) : value(p) 
        {
            std::println("Future constructed.");
        }
        ~MyFuture() 
        {
            std::println("Future destroyed.");
        }
        T get() 
        {                                          // (10)
            return *value;
        }

        struct promise_type 
        {
            std::shared_ptr<T> ptr = std::make_shared<T>(); // (4)
            ~promise_type() 
            {
                std::println("Promise-type destroyed.");
            }
            MyFuture<T> get_return_object() 
            {              // (7)
                std::println("get_return_object().");
                return ptr;
            }
            void return_value(T v) 
            {
                std::println("return_value().");
                *ptr = v;
            }
            std::suspend_never initial_suspend() 
            {          // (5)
                std::println("initial_suspend().");
                return {};
            }
            std::suspend_never final_suspend() noexcept 
            {  // (6)
                std::println("final_suspend().");
                return {};
            }
            void unhandled_exception() 
            {
                std::println("unhandled_exception().");
                std::exit(1);
            }
        };
    };

    MyFuture<int> createFuture() 
    {                         // (1)
        co_return 2021;                                    // (9)
    }

    void RunSample() 
    {
        auto fut = createFuture();
        std::println("fut.get(): {}", fut.get());   // (2)
    }
}

export namespace Coroutines::Events
{
    struct BasicCoroutine 
    {
        struct Promise {
            BasicCoroutine get_return_object() 
            { 
                return BasicCoroutine{}; 
            }

            void unhandled_exception() noexcept {}

            void return_void() noexcept {}

            std::suspend_never initial_suspend() noexcept { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
        };
        using promise_type = Promise;
    };

    struct IoResult {
        std::error_code ec;
        int result;
    };

    struct RecvFromAwaitable 
    {
        struct Awaiter
        {
            Awaiter(RecvFromAwaitable& e) : awaitable{ e } {}

            RecvFromAwaitable& awaitable;
            IoResult result = {};

            bool await_ready() const noexcept { return false; }

            void await_suspend(std::coroutine_handle<> handle) noexcept
            {

            }

            IoResult await_resume() const noexcept { return result; }
        };

        RecvFromAwaitable()
        {
        }
        

        auto operator co_await()
        {
            return Awaiter{ *this };
        }

    private:

    };

    BasicCoroutine serve()
    {
        while (true) {
            std::string receiveBuffer(1024, '\0');
            const auto [recvEc, receivedBytes] = co_await RecvFromAwaitable();
            if (recvEc) {
                continue;
            }
            receiveBuffer.resize(receivedBytes);
        }
    }

    void RunSample()
    {
        serve();
    }
}

//https://lewissbaker.github.io/2017/11/17/understanding-operator-co-await
export namespace Coroutines::ManualResetEvent
{
    struct async_manual_reset_event
    {
        struct awaiter
        {
            awaiter(const async_manual_reset_event& event) noexcept
                : m_event(event)
            {
            }

            bool await_ready() const noexcept
            {
                return m_event.is_set();
            }
            bool await_suspend(std::coroutine_handle<> awaitingCoroutine) noexcept
            {
                // Special m_state value that indicates the event is in the 'set' state.
                const void* const setState = &m_event;

                // Remember the handle of the awaiting coroutine.
                m_awaitingCoroutine = awaitingCoroutine;

                // Try to atomically push this awaiter onto the front of the list.
                void* oldValue = m_event.m_state.load(std::memory_order_acquire);
                do
                {
                    // Resume immediately if already in 'set' state.
                    if (oldValue == setState) return false;

                    // Update linked list to point at current head.
                    m_next = static_cast<awaiter*>(oldValue);

                    // Finally, try to swap the old list head, inserting this awaiter
                    // as the new list head.
                } while (!m_event.m_state.compare_exchange_weak(
                    oldValue,
                    this,
                    std::memory_order_release,
                    std::memory_order_acquire));

                // Successfully enqueued. Remain suspended.
                return true;
            }
            void await_resume() noexcept {}

            const async_manual_reset_event& m_event;
            std::coroutine_handle<> m_awaitingCoroutine;
            awaiter* m_next;
        };

        async_manual_reset_event(bool initiallySet) noexcept
            : m_state(initiallySet ? this : nullptr)
        {
        }

        // No copying/moving
        async_manual_reset_event(const async_manual_reset_event&) = delete;
        async_manual_reset_event(async_manual_reset_event&&) = delete;
        async_manual_reset_event& operator=(const async_manual_reset_event&) = delete;
        async_manual_reset_event& operator=(async_manual_reset_event&&) = delete;

        bool is_set() const noexcept
        {
            return m_state.load(std::memory_order_acquire) == this;
        }

        awaiter operator co_await() const noexcept
        {
            return awaiter{ *this };
        }

        void set() noexcept
        {
            // Needs to be 'release' so that subsequent 'co_await' has
            // visibility of our prior writes.
            // Needs to be 'acquire' so that we have visibility of prior
            // writes by awaiting coroutines.
            void* oldValue = m_state.exchange(this, std::memory_order_acq_rel);
            if (oldValue != this)
            {
                // Wasn't already in 'set' state.
                // Treat old value as head of a linked-list of waiters
                // which we have now acquired and need to resume.
                auto* waiters = static_cast<awaiter*>(oldValue);
                while (waiters != nullptr)
                {
                    // Read m_next before resuming the coroutine as resuming
                    // the coroutine will likely destroy the awaiter object.
                    auto* next = waiters->m_next;
                    waiters->m_awaitingCoroutine.resume();
                    waiters = next;
                }
            }
        }
        void reset()  noexcept
        {
            void* oldValue = this;
            m_state.compare_exchange_strong(oldValue, nullptr, std::memory_order_acquire);
        }

    private:
        // - 'this' => set state
        // - otherwise => not set, head of linked list of awaiter*.
        mutable std::atomic<void*> m_state;
    };

    struct task
    {
        struct promise_type
        {
            task get_return_object() { return {}; }
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
            void return_void() {}
            void unhandled_exception() {}
        };
    };

    task example(async_manual_reset_event& event)
    {
        co_await event;
    }

    void Run()
    {
        async_manual_reset_event a{ false };
        auto t = example(a);
        a.is_set();
    }
}





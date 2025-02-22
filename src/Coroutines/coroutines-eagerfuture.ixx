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

export namespace Coroutines::LazyFuture
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
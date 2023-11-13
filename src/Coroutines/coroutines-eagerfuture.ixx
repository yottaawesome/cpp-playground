// Adapted from https://www.modernescpp.com/index.php/implementing-futures-with-coroutines
export module coroutines:eagerfuture;
import std;

// eagerFuture.cpp

export namespace Coroutines::EagerFuture
{
    template<typename T>
    struct MyFuture {
        std::shared_ptr<T> value;                           // (3)
        MyFuture(std::shared_ptr<T> p) : value(p) {}
        ~MyFuture() { }
        T get() {                                          // (10)
            return *value;
        }

        struct promise_type {
            std::shared_ptr<T> ptr = std::make_shared<T>(); // (4)
            ~promise_type() { }
            MyFuture<T> get_return_object() {              // (7)
                return ptr;
            }
            void return_value(T v) {
                *ptr = v;
            }
            std::suspend_never initial_suspend() {          // (5)
                return {};
            }
            std::suspend_never final_suspend() noexcept {  // (6)
                return {};
            }
            void unhandled_exception() {
                std::exit(1);
            }
        };
    };

    MyFuture<int> createFuture() {                         // (1)
        co_return 2021;                                    // (9)
    }

    void RunSample() 
    {
        std::cout << '\n';
        auto fut = createFuture();
        std::cout << "fut.get(): " << fut.get() << '\n\n';   // (2)
    }
}
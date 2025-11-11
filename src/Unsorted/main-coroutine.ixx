export module main:coroutine;
import std;

namespace FileCoroutine1
{
    template<typename T>
    struct Generator
    {
        struct promise_type
        {
            T current_value{};

            auto initial_suspend() noexcept -> std::suspend_always 
            { 
                return {}; 
            }
            
            auto final_suspend() noexcept -> std::suspend_always 
            { 
                return {}; 
            }

            auto get_return_object() noexcept
            {
                return Generator{ std::coroutine_handle<promise_type>::from_promise(*this) };
            }

			auto yield_value(T value) noexcept -> std::suspend_always
            {
                current_value = std::move(value);
                return {};
            }

            void unhandled_exception() 
            { 
                std::rethrow_exception(std::current_exception()); 
            }
            
            void return_void() noexcept {}
        };

        using handle_t = std::coroutine_handle<promise_type>;

        Generator() = default;
        explicit Generator(handle_t h) : coro(h) {}

        Generator(const Generator&) = delete;
        Generator& operator=(const Generator&) = delete;

        Generator(Generator&& other) noexcept 
            : coro(std::exchange(other.coro, {}))
        {}

        auto operator=(Generator&& other) noexcept -> Generator&
        {
            if (this != &other)
            {
                if (coro) 
                    coro.destroy();
                coro = std::exchange(other.coro, {});
            }
            return *this;
        }

        ~Generator()
        {
            if (coro) 
                coro.destroy();
        }

        struct iterator
        {
            handle_t coro{};
            bool done = true;

            iterator() = default;
            explicit iterator(handle_t h) : coro(h), done(!h || h.done()) {}

            auto operator++() -> iterator&
            {
                coro.resume();
                done = coro.done();
                return *this;
            }

            auto operator*() const -> const T&
            { 
                return coro.promise().current_value; 
            }

            auto operator==(std::default_sentinel_t) const -> bool
            { 
                return done; 
            }
        };

        auto begin() -> iterator
        {
            if (not coro)
                return iterator{};
            coro.resume();
            return iterator{ coro };
        }

        auto end() const noexcept -> std::default_sentinel_t { return {}; }

    private:
        handle_t coro{};
    };

	inline auto read_lines(const std::filesystem::path& path) -> Generator<std::string>
    {
        std::ifstream in(path);
        if (!in.is_open())
        {
            co_return;
        }

        std::string line;
        while (std::getline(in, line))
        {
            co_yield line;
        }
    }

    void Run()
    {
        for (const auto& line : read_lines("test.txt"))
        {
            std::println("{}", line);
        }
    }
}

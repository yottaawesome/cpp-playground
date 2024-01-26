module;

// Moved to own module, was causing issues with std imports in other modules
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

export module moremadness;
import std;
import std.compat;

export namespace WaitingB
{
    template<typename T>
    struct is_array : std::false_type {};

    template<typename T, size_t N>
    struct is_array<std::array<T, N>> : std::true_type {};

    template<typename T, size_t N>
    constexpr bool is_array_v = is_array<T, N>;

    template<typename T>
    concept array_like = is_array<std::remove_cvref_t<T>>::value;

    constexpr auto a = std::array<int, 3>{1, 2, 3};
    static_assert(array_like<decltype(a)>);

    template<class T>
    struct is_duration : std::false_type {};

    template<class Rep, class Period>
    struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type {};

    template<typename T>
    concept duration = is_duration<T>::value;

    template<typename T>
    concept waitable =
        not std::is_null_pointer_v<T>
        and (
            std::same_as<std::remove_cvref_t<T>, void*>
            //or array_like<T>
            or requires(const T t)
    {
        {t.get_handle()} noexcept -> std::same_as<void*>;
    }
    );

    template<typename TDummy>
    class handle_t final
    {
        public:
            ~handle_t() { close(); }

            handle_t() noexcept = default;

            handle_t(const handle_t&) = delete;
            handle_t operator=(const handle_t&) = delete;
            handle_t(handle_t&& other) { move(other); }
            handle_t operator=(handle_t&& other) { move(other); }

        public:
            void* get_handle() const noexcept { return m_handle; }

        private:
            void close() noexcept
            {
                if (not m_handle)
                    return;
                CloseHandle(m_handle);
                m_handle = nullptr;
            }

            void move(handle_t& other) noexcept
            {
                close();
                m_handle = other.m_handle;
                other.m_handle = nullptr;
            }

        private:
            void* m_handle =
                []() noexcept
                {
                    void* handle = CreateEvent(nullptr, true, false, nullptr);
                    if (not handle)
                        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
                    return handle;
                }();
    };

    using handle_a = handle_t<struct handle_a_d>;
    using handle_b = handle_t<struct handle_b_d>;
    static_assert(waitable<handle_b> and waitable<handle_a>);

    template<typename T>
    struct false_type : std::false_type {};

    template<waitable T>
    constexpr auto to_handle(T&& item)
    {
        using TU = std::remove_cvref_t<T>;
        if constexpr (std::same_as<TU, void*>)
            return item;
        else if constexpr (array_like<TU>)
            return item;
        else
            return item.get_handle();
    }

    auto create_array(auto&&...args)
    {
        /*if constexpr (sizeof...(args) ==  1)
        {
            static_assert()
        }*/
    }

    unsigned wait_on(
        const bool wait_for_all,
        const bool is_interruptible_wait,
        const duration auto timespan,
        waitable auto&&... waitables
    )
    {
        static_assert(sizeof...(waitables) > 0, "Must pass at least one waitable.");
        static_assert(sizeof...(waitables) <= MAXIMUM_WAIT_OBJECTS, "Cannot wait on more than MAXIMUM_WAIT_OBJECTS objects.");

        const auto waitable_array = std::array<void*, sizeof...(waitables)>{ to_handle(waitables)... };
        if (std::any_of(waitable_array.begin(), waitable_array.end(), [](auto&& h) { return h == nullptr; }))
            throw std::runtime_error("Unexpected nullptr for handle");

        using std::chrono::duration_cast, std::chrono::milliseconds;
        const unsigned result = WaitForMultipleObjectsEx(
            static_cast<unsigned>(waitable_array.size()),
            waitable_array.data(),
            wait_for_all,
            static_cast<unsigned>(duration_cast<milliseconds>(timespan).count()),
            is_interruptible_wait
        );
        if (result == WAIT_ABANDONED)
            throw std::runtime_error("The wait was unexpectedly abandoned");
        if (result == WAIT_FAILED)
            throw std::runtime_error("The wait unexpectedly failed");

        return result;
    }

    template<typename T, size_t N>
    auto array_to_tuple(const std::array<T, N>& arr)
    {
        // std::tuple_size_v<std::remove_cvref_t<decltype(arr)>> works too
        return[]<size_t...Is>(const std::array<T, N>&arr, std::index_sequence<Is...>)
        {
            return std::tuple{ arr[Is]... };
        }(arr, std::make_index_sequence<N>{});
    }

    unsigned wait_on(
        const bool wait_for_all,
        const bool is_interruptible_wait,
        const duration auto timespan,
        array_like auto&& waitables
    )
    {
        return std::apply(
            [wait_for_all, is_interruptible_wait, timespan]<typename...T>(T&&...n)
        {
            return wait_on(wait_for_all, is_interruptible_wait, timespan, std::forward<T>(n)...);
        },
            waitables
        );
    }

    void Random()
    {
        std::index_sequence<3> arr[3];

        auto x = []<size_t...Is>(std::index_sequence<Is...>)
        {
            return (
                []<size_t I = Is>()
            {
                if constexpr (I == 1)
                {

                }
            }(), ...
                );
        };
        x(std::make_index_sequence<3>{});
    }

    void Run()
    {
        handle_a a{};
        handle_b b{};
        std::jthread t(
            [](handle_a& a)
            {
                std::this_thread::sleep_for(std::chrono::seconds{ 1 });
                std::println("Event signaled succesfully");
                SetEvent(a.get_handle());
            },
            std::ref(a)
        );

        unsigned result = wait_on(false, false, std::chrono::seconds{ 5 }, a, b);
        unsigned result2 = wait_on(false, false, std::chrono::seconds{ 5 }, std::array<void*, 2>{nullptr, nullptr});
        if (result == WAIT_TIMEOUT)
            std::println("Timed out");
        else if (result == 0)
            std::println("Wait one succeeded");
    }
}

export namespace AnotherWait
{
    template<class T>
    struct is_duration : std::false_type {};

    template<class Rep, class Period>
    struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type {};

    template<typename T>
    concept duration = is_duration<T>::value;

    template<typename T>
    concept waitable =
        not std::is_null_pointer_v<T>
        and (
            std::same_as<std::remove_cvref_t<T>, void*>
            or requires(const T t)
    {
        {t.get_handle()} noexcept -> std::same_as<void*>;
    }
    );

    template<typename T>
    struct is_vector : std::false_type {};

    template<typename T>
    struct is_vector<std::vector<T>> : std::true_type {};

    template<typename T>
    constexpr bool is_vector_v = is_vector<std::vector<T>>::value;

    template<typename T>
    struct is_array : std::false_type {};

    template<typename T, size_t N>
    struct is_array<std::array<T, N>> : std::true_type {};

    template<typename T, size_t N>
    constexpr bool is_array_v = is_array<T, N>;

    template<typename T>
    concept arraylike = is_array_v<T>;

    template<waitable T>
    constexpr auto to_handle(T&& item)
    {
        using TU = std::remove_cvref_t<T>;
        if constexpr (std::same_as<TU, void*>)
            return item;
        else
            return item.get_handle();
    }

    unsigned internal_wait_on(
        const bool wait_for_all,
        const bool is_interruptible_wait,
        const duration auto timespan,
        const void* waitables,
        const size_t count
    )
    {
        const unsigned result = WaitForMultipleObjectsEx(
            static_cast<unsigned>(count),
            static_cast<const HANDLE*>(waitables),
            wait_for_all,
            static_cast<unsigned>(std::chrono::duration_cast<std::chrono::milliseconds>(timespan).count()),
            is_interruptible_wait
        );
        if (result == WAIT_ABANDONED)
            throw std::runtime_error("The wait was unexpectedly abandoned");
        if (result == WAIT_FAILED)
            throw std::runtime_error("The wait unexpectedly failed");

        return result;
    }

    template<typename T>
    constexpr bool not_array(T&& t)
    {
        return not arraylike<std::remove_cvref_t<T>>;
    }

    constexpr bool not_array_one(auto&&...waitables)
    {
        if constexpr (sizeof...(waitables) > 1)
        {
            constexpr bool b = []<size_t...Is, typename TTuple>(std::index_sequence<Is...>, TTuple && args)
            {
                return (not_array(std::get<Is>(args)) and ...);
            }(
                std::make_index_sequence<sizeof...(waitables)>{},
                std::forward_as_tuple(waitables...)
                );
            static_assert(b);
            return b;
        }
        else
        {
            return true;
        }
    }

    void T()
    {
        if constexpr (not_array(std::array{ 1,2 }))
        {

        }
    }

    unsigned wait_on(
        const bool wait_for_all,
        const bool is_interruptible_wait,
        const duration auto timespan,
        waitable auto&&... waitables
    )
    {
        static_assert(sizeof...(waitables) > 0, "Must pass at least one waitable.");
        static_assert(sizeof...(waitables) <= MAXIMUM_WAIT_OBJECTS, "Cannot wait on more than MAXIMUM_WAIT_OBJECTS objects.");

        const auto waitable_array = std::array<void*, sizeof...(waitables)>{ to_handle(waitables)... };
        if (std::any_of(waitable_array.begin(), waitable_array.end(), [](auto&& h) { return h == nullptr; }))
            throw std::runtime_error("Unexpected nullptr for handle");

        return internal_wait_on(
            wait_for_all,
            is_interruptible_wait,
            timespan,
            waitable_array.data(),
            waitable_array.size()
        );
    }

    template<size_t N>
    unsigned wait_on(
        const bool wait_for_all,
        const bool is_interruptible_wait,
        const duration auto timespan,
        const std::array<void*, N>& waitables
    )
    {
        return internal_wait_on(wait_for_all, is_interruptible_wait, timespan, waitables.data(), N);
    }

    unsigned wait_on(
        const bool wait_for_all,
        const bool is_interruptible_wait,
        const duration auto timespan,
        const std::vector<void*>& waitables
    )
    {
        return internal_wait_on(wait_for_all, is_interruptible_wait, timespan, waitables.data(), waitables.size());
    }

    void Run()
    {
        void* a = CreateEvent(nullptr, true, false, nullptr);
        void* b = CreateEvent(nullptr, true, false, nullptr);
        wait_on(false, false, std::chrono::seconds{ 2 }, std::vector{ a, b });
        wait_on(false, false, std::chrono::seconds{ 2 }, std::array{ a, b });
        wait_on(false, false, std::chrono::seconds{ 2 }, a);
    }
}
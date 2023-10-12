export module demo;
import std;
import win32;

export namespace WithTuple
{
    class EventA
    {
        public:
            Win32::HANDLE Handle;
            void operator()()
            {
                std::cout << "Event A invoked...\n";
            }
    };

    class EventB
    {
        public:
            Win32::HANDLE Handle;
            void operator()()
            {
                std::cout << "Event B invoked...\n";
            }
    };

    // https://stackoverflow.com/a/8866219
    template <class T, class M> M get_member_type(M T::*);

    template<typename T>
    concept EventInvocable = requires(T t)
    {
        //using x = decltype(get_member_type(&T::Handle));
        requires std::is_same_v< decltype(get_member_type(&T::Handle)), Win32::HANDLE>;
        t();
    };

    // Three ways of constained function signatures
    EventInvocable auto SomeFunc1();
    auto SomeFunc2() -> EventInvocable auto;
    EventInvocable auto SomeFunc3() -> auto;

    template <class Tuple, size_t N = 0>
    auto RuntimeInvoke(Tuple& tup, size_t idx)
    {
        if (N == idx)
        {
            return std::get<N>(tup)();
        }

        if constexpr (N + 1 < std::tuple_size_v<Tuple>)
        {
            return RuntimeInvoke<Tuple, N + 1>(tup, idx);
        }
    }

    template<EventInvocable...E>
    class EventHandler
    {
        public:
            EventHandler(E&...args)
                : m_handles{ std::forward<E>(args)... }
            {
                std::apply(
                    [this](auto&&... val)
                    {
                        m_winhandles = std::array{ std::forward<Win32::HANDLE>(val.Handle) ... };
                    },
                    m_handles
                );
            }

        public:
            void Wait()
            {
                auto index = Win32::WaitForMultipleObjects(
                    static_cast<DWORD>(m_winhandles.size()),
                    m_winhandles.data(),
                    false,
                    Win32::WaitInfinite
                );
                if (index >= m_winhandles.size())
                    throw std::runtime_error(std::format("Error waiting on object {}", index));
                RuntimeInvoke(m_handles, index);
            }

        private:
            std::tuple<E...> m_handles;
            std::array<Win32::HANDLE, sizeof...(E)> m_winhandles;
    };

    void func(int x, int y)
    {

    }

	int Run()
	{
        std::tuple sometup{ 1,2 };
        std::apply(func, sometup);

        Win32::HANDLE h_a = Win32::CreateEventW(
            nullptr,
            false,
            false,
            nullptr
        );
        if (not h_a)
            return 1;

        Win32::HANDLE h_b = Win32::CreateEventW(
            nullptr,
            false,
            false,
            nullptr
        );
        if (not h_b)
            return 2;

        EventA a{
            .Handle = h_a
        };
        EventB b{
            .Handle = h_b
        };

        EventHandler e(a, b);
        Win32::SetEvent(h_a);
        e.Wait();
        Win32::SetEvent(h_b);
        e.Wait();

        return 0;
	}
}
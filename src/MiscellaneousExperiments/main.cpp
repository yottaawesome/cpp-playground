#include <windows.h>
import std;
import std.compat;

namespace Random
{
    template<typename T>
    class Q
    {
        friend T;

        public:
            Q(T& t) : m_t(t) {}

        public:
            void DoThis() { m_t.DoSomething(*this); }

        private:
            int M;
            T m_t;
    };

    template<auto T>
    class Q2
    {
        friend decltype(T);

        public:
            void DoThis() { T.DoSomething(*this); }

        private:
            int M;
    };

    class M
    {
        void DoSomething(Q<M>& m)
        {
            m.M = 1;
        }
    };

    struct Non
    {
        Non() = default;
        Non(const Non&) = delete;
        Non operator=(const Non&) = delete;

        Non(Non&&) noexcept = default;
    };
}

namespace Random2
{
    template <size_t N>
    struct FixedString
    {
        // There's a consteval bug in the compiler.
        // See https://developercommunity.visualstudio.com/t/consteval-function-unexpectedly-returns/10501040
        wchar_t buf[N]{};

        constexpr FixedString(const wchar_t(&arg)[N]) noexcept
        {
            std::copy_n(arg, N, buf);
        }

        constexpr operator const wchar_t* () const noexcept
        {
            return buf;
        }

        constexpr operator std::wstring_view() const noexcept
        {
            return { buf, N };
        }

        operator std::wstring() const noexcept
        {
            return { buf, N };
        }

        std::wstring ToString() const noexcept
        {
            return { buf, N };
        }
    };
    template<size_t N>
    FixedString(wchar_t const (&)[N]) -> FixedString<N>;

    struct MessageAndLocation
    {
        std::string_view message;
        std::source_location loc;

        template<typename T>
        MessageAndLocation(
            T&& msg,
            std::source_location loc = std::source_location::current()
        ) requires std::is_convertible_v<T, std::string_view>
            : message{ msg },
            loc{ loc }
        {}
    };

    void ErrorCheck(bool condition, MessageAndLocation mal, auto&&...formatArgs)
    {
        if (!condition)
        {
            throw std::runtime_error(
                std::vformat(
                    mal.message, 
                    std::make_format_args(std::forward<decltype(formatArgs)>(formatArgs)...))
            );
        }
    }

    template<auto F>
    void ErrorCheck2(auto value, MessageAndLocation mal, auto&&...formatArgs)
    {
        if (!F(value))
        {
            throw std::runtime_error(
                std::vformat(
                    mal.message,
                    std::make_format_args(std::forward<decltype(formatArgs)>(formatArgs)...))
            );
        }
    }

    template<auto F, typename E>
    struct ErrorCheck3
    {
        static std::string Format(
            MessageAndLocation msgAndLoc,
            auto&&...formatArgs
        ) noexcept try
        {
            std::string baseMessage = std::vformat(
                msgAndLoc.message,
                std::make_format_args(std::forward<decltype(formatArgs)>(formatArgs)...));

            std::string_view fileName{ msgAndLoc.loc.file_name() };
            if (fileName.contains('\\') and not fileName.ends_with('\\'))
                fileName = { &fileName[fileName.find_last_of('\\') + 1] };

            constexpr std::string_view completeMessage = "Error: '{}' --> [{} @ {}:{}:{}].\n";
            return std::vformat(
                completeMessage,
                std::make_format_args(baseMessage, msgAndLoc.loc.function_name(), fileName, msgAndLoc.loc.line(), msgAndLoc.loc.column())
            );
        }
        catch (...)
        {
            return {};
        }

        void operator()(auto value, MessageAndLocation msgAndLoc, auto&&...formatArgs) const
        {
            if (F(value))
                return;
            throw E(Format(msgAndLoc, std::forward<decltype(formatArgs)>(formatArgs)...));
        }
    };

    using RuntimeErrorWinCheck = ErrorCheck3<[](auto value) { return value == 0; }, std::runtime_error>;
    constexpr RuntimeErrorWinCheck NotSuccess;
}

int main()
{
    try
    {
        Random2::NotSuccess(1, "Something went wrong: {}", "who knows?");
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what();
    }
    //M m;
    //Q<M> qm(m);
    return 0;
}

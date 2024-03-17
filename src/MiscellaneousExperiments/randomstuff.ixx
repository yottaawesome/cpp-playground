module;

#include <windows.h>

export module randomstuff;
import std;
import std.compat;

export namespace Random
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

    void Run()
    {
        M m;
        Q<M> qm(m);
    }
}

export namespace Random2
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

    using RuntimeErrorWinCheck = ErrorCheck3 < [](auto value) { return value == 0; }, std::runtime_error > ;
    constexpr RuntimeErrorWinCheck NotSuccess;

    void Run()
    {
        try
        {
            Random2::NotSuccess(1, "Something went wrong: {}", "who knows?");
        }
        catch (const std::exception& ex)
        {
            std::cout << ex.what();
        }
    }
}

export namespace IfDefined
{
    constexpr bool IsConditionTrue() noexcept
    {
        return true;
    }

    template<typename T>
    concept IsDefined = requires (T t)
    {
        true;
    };

    template<auto T>
    concept IsDefinedV = requires()
    {
        requires std::is_invocable_v<decltype(T)>;
    };

    void b();

    class M;

    // See Raymond Chen's series on this topic.
    // https://devblogs.microsoft.com/oldnewthing/20190708-00/?p=102664
    // https://devblogs.microsoft.com/oldnewthing/20190709-00/?p=102671
    // https://devblogs.microsoft.com/oldnewthing/20190710-00/?p=102678
    // https://devblogs.microsoft.com/oldnewthing/20190711-00/?p=102682
    // https://devblogs.microsoft.com/oldnewthing/20190712-00/?p=102690
    template<typename, typename = void>
    constexpr bool is_type_complete_v = false;

    template<typename T>
    constexpr bool is_type_complete_v
        <T, std::void_t<decltype(sizeof(T))>> = true;

    template<typename... T, typename TLambda>
    void call_if_defined(TLambda&& lambda)
    {
        if constexpr ((... && is_type_complete_v<T>))
        {
            lambda(static_cast<T*>(nullptr)...);
        }
    }

    void Run()
    {
        constexpr bool v1 = IsDefinedV<b>;
        constexpr bool v2 = is_type_complete_v<M>;
    }
}

export namespace Span
{
    void Run()
    {
        std::vector<int> v{ 1,2,3,4,5 };
        std::span<int> s{ v.begin(), 2};
    }
}

// https://andreasfertig.blog/2024/01/cpp20-concepts-applied/
export namespace BitfieldEnums
{
    template<typename T>
    concept op_or = std::is_enum_v<T> and requires(T e)
    {
        //look for enable_bitmask_operator_or to enable this operator
        enable_bitmask_operator_or(e);
    };

    template<typename T>
        requires op_or<T>
    constexpr auto operator|(const T lhs, const T rhs)
    {
        return static_cast<T>(std::to_underlying(lhs) | std::to_underlying(rhs));
    }

    namespace Filesystem
    {
        enum class Permission : uint8_t
        {
            Read = 0x01,
            Write = 0x02,
            Execute = 0x04,
        };

        //Opt - in for operator|
        consteval void enable_bitmask_operator_or(Permission);
    }

    using Filesystem::Permission;
    Permission readAndWrite{ Permission::Read | Permission::Write };
}

export namespace DeducingThis
{
    void Run();
}

export namespace sums
{
    // See https://www.cppstories.com/2022/20-smaller-cpp20-features/
    constexpr auto constexpr_sum()
    {
        std::vector<int> someNumbers{ 1,2,3,4 };
        auto tmp = std::accumulate(someNumbers.begin(), someNumbers.end(), 0);
        return tmp;
    }

    consteval auto consteval_sum()
    {
        return constexpr_sum();
    }

    void Run()
    {
        std::println("{}", consteval_sum());
    }
}

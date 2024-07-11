module;

#include <windows.h>

export module randomstuff;
import std;
import std.compat;
export import :dates;

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
    struct MessageAndLocation
    {
        std::string_view message;
        std::source_location loc;

        template<typename T>
        MessageAndLocation(T&& msg, std::source_location loc = std::source_location::current()) 
            requires std::is_convertible_v<T, std::string_view>
            : message{ msg }, loc{ loc }
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
                [](auto&&...args) 
                {
                    return std::make_format_args(args...);
                }(baseMessage, msgAndLoc.loc.function_name(), fileName, msgAndLoc.loc.line(), msgAndLoc.loc.column())
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

export namespace dont_return_protected_reference
{
    // ICE
    /*constexpr int blah()
    {
        try
        {
            return 1;
        }
        catch (...)
        {
            return 2;
        }
    }*/

    template<typename TFunc, typename S, typename...TArgs>
    concept basic_invocable =
        std::invocable<TFunc, S, TArgs...>
        //and not std::is_reference_v<std::invoke_result_t<T>>
        ;

    // std::remove_cv_t doesn't work as it only removes topmost qualifiers
    template<typename TFn, typename S, typename...TArgs, typename TFnResult = std::remove_volatile_t<std::invoke_result_t<TFn, S, TArgs...>>>
    concept restricted_function =
        std::invocable<TFn, S, TArgs...>
        and not std::same_as<S&, TFnResult>
        and not std::same_as<const S&, TFnResult>
        and not std::same_as<S*, TFnResult>
        and not std::same_as<const S*, TFnResult>
        and not std::same_as<S* const, TFnResult>
        //and not std::is_reference_v<std::invoke_result_t<T>>
        ;

    template<typename T>
    struct protect
    {
        template<typename...TArgs>
        auto invoke_on(restricted_function<T, TArgs> auto&& func, TArgs&&...args)
        {
            return std::invoke(func, to_protect, std::forward<TArgs>(args)...);
        }
        // We want to protect this field and so we don't want functions returning 
        // a reference or a pointer to it.
        T to_protect;
    };

    struct Y {};

    void run()
    {
        protect<int> p;
        const int& x = p.invoke_on(
            [](int i, Y y) -> int
            {
                return i;
            },
            Y{}
        );
    }

    template<typename...TArgs>
    struct BaseVariant
    {

    };
}

export namespace SettingsTest
{
    template<auto VInvoke>
    struct Setting
    {
        using TReturn = std::invoke_result_t<decltype(VInvoke)>;

        operator TReturn() const noexcept(std::is_nothrow_invocable_v<decltype(VInvoke)>)
        {
            return std::invoke(VInvoke);
        }
    };

    template<auto VInvoke, bool VEagerInitialisation>
    struct CachedSetting
    {
        protected:
        using TInvoke = decltype(VInvoke);
        using TReturn = std::invoke_result_t<TInvoke>;

        public:
        constexpr CachedSetting() requires (not VEagerInitialisation) = default;
        CachedSetting() requires VEagerInitialisation { Refresh(); }

        operator TReturn() noexcept(NoExcept)
        {
            return Get();
        }

        TReturn Get() noexcept(NoExcept)
        {
            if constexpr (not VEagerInitialisation)
                if (not m_cached.has_value()) // Uncomment to disable lazy initialisation
                    Refresh();

            return m_cached.value();
        }

        protected:
        void Refresh() noexcept(NoExcept)
        {
            m_cached = std::invoke(VInvoke);
        }

        static constexpr bool NoExcept = std::is_nothrow_invocable_v<TInvoke>;
        std::optional<TReturn> m_cached;
    };

    template<auto VInvoke, int VTimeout, bool VEagerInitialisation>
    struct TimedCachedSetting
    {
        private:
        using TInvoke = decltype(VInvoke);
        using TReturn = std::invoke_result_t<TInvoke>;
        
        public:
        constexpr TimedCachedSetting() requires (not VEagerInitialisation) = default;
        TimedCachedSetting() requires VEagerInitialisation { Refresh(); }

        operator TReturn() noexcept(NoExcept)
        {
            return Get();
        }

        std::chrono::steady_clock::time_point LastRefresh() const noexcept
        {
            return m_lastRefresh;
        }

        TReturn Get() noexcept(NoExcept)
        {
            if constexpr (not VEagerInitialisation)
                if (not m_cached.has_value()) // Uncomment to disable lazy initialisation
                    Refresh();
            
            if (auto dur = std::chrono::steady_clock::now() - m_lastRefresh; std::chrono::duration_cast<std::chrono::seconds>(dur) > Timeout)
                Refresh();
            return m_cached.value();
        }

        private:
        void Refresh() noexcept(NoExcept)
        {
            m_cached = std::invoke(VInvoke);
            m_lastRefresh = std::chrono::steady_clock::now();
        }

        static constexpr bool NoExcept = std::is_nothrow_invocable_v<TInvoke>;
        static constexpr std::chrono::seconds Timeout{ VTimeout };
        std::chrono::steady_clock::time_point m_lastRefresh;
        std::optional<TReturn> m_cached;
    };

    int Something() { return 1; }

    constexpr Setting<[] { return 1; }> Blah;
    // Adding constexpr causes ICE.
    TimedCachedSetting<[] { return 1; }, 30, true> Blah2;
    constexpr Setting<Something> Blah3;
    TimedCachedSetting<[] { return 1; }, 30, false> Blah4;
    CachedSetting<[]{ return 1; }, false> Blah5;

    template<auto VGet, auto VSet = nullptr>
    struct Settable
    {
        using TGet = decltype(VGet);
        using TSet = decltype(VSet);
        using TReturn = std::invoke_result_t<TGet>;
        using TGetExpected = std::expected<TReturn, std::string>;
        constexpr Settable() = default;

        operator TReturn() const
        {
            return std::invoke(VGet);
        }

        auto operator()() const
        {
            return std::invoke(VGet);
        }

        TGetExpected operator()(const std::nothrow_t&) const noexcept
        try
        {
            return std::invoke(VGet);
        }
        catch (const std::exception& ex)
        {
            return std::unexpected(ex.what());
        }

        template<typename...TArgs>
        auto operator()(TArgs&&...set) const
            requires std::invocable<TSet, TArgs...>
        {
            return Set(std::forward<TArgs>(set)...);
        }

        template<typename...TArgs>
        const auto& operator=(TArgs&&...set) const 
            requires std::invocable<TSet, TArgs...>
        {
            Set(std::forward<TArgs>(set)...);
            return *this;
        }

        template<typename...TArgs>
        auto Set(TArgs&&...set) const
        {
            return std::invoke(VSet, std::forward<TArgs>(set)...);
        }

        template<typename...TArgs>
        auto Get(TArgs&&...set) const
            requires std::invocable<TGet, TArgs...>
        {
            return std::invoke(VGet);
        }
    };

    constexpr Settable<
        []() -> std::string 
        { 
            return "a";
        }, 
        [](std::string_view i) 
        {
        }
    > TestSettable;

    constexpr Settable <
        []() -> std::string
        {
            return "a";
        }
    > TestSettable2;

    void Run()
    {
        int x1 = Blah;
        int x2 = Blah2;
        int x3 = Blah3;
        TestSettable = "a";
        std::string x5 = TestSettable2;
        std::string x4 = TestSettable;
    }
}

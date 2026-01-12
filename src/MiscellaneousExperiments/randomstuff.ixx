module;

#include <windows.h>

export module randomstuff;
import std;
import std.compat;
export import :dates;
export import :stringconversion;
export import :altvariant;

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

export namespace ProperMessageAndLocation
{
    constexpr auto Format = "{}\n    ['{}' in {}:{}].";

    template<typename...TArgs>
    struct MessageAndLocation
    {
        std::format_string<TArgs...> Fmt;
        std::source_location Location;

        consteval MessageAndLocation(auto&& fmt, std::source_location loc = std::source_location::current()) : Fmt(fmt), Location(loc) {};
    };

    template<typename...TArgs>
    constexpr auto FMT(MessageAndLocation<std::type_identity_t<TArgs>...> m, TArgs&&...args)
    {
        return std::format(
            Format,
            std::format(m.Fmt, std::forward<TArgs>(args)...),
            m.Location.function_name(),
            m.Location.file_name(),
            m.Location.line());
    }

    void Run()
    {
        FMT("What is this {}", 10);
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
        auto invoke_on(restricted_function<T, TArgs...> auto&& func, TArgs&&...args)
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

    // ICE
    template<auto VGet>
    struct SettableDefaults
    {
        static constexpr auto VSet = nullptr;
        static std::expected<std::invoke_result_t<decltype(VGet)>, std::string> VNoExceptGet()
        {
            try
            {
                return std::invoke(VGet);
            }
            catch (const std::exception& ex)
            {
                return std::unexpected(ex.what());
            }
        };
    };

    template<auto VGet>
    constexpr auto V =
        []() noexcept -> std::expected<std::invoke_result_t<decltype(VGet)>, std::string>
        {
            try
            {
                return std::invoke(VGet);
            }
            catch (const std::exception& ex)
            {
                return std::unexpected(ex.what());
            }
        };

    template<
        auto VGet, 
        auto VSet = nullptr,
        auto VNoExceptGet = V<VGet>
    >
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

        auto operator()(const std::nothrow_t&) const noexcept { return VNoExceptGet(); }

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

export namespace InheritanceTags
{
    struct Human {};
    struct Enemy {};

    struct Johnson : Human, Enemy
    {

    };

    template<typename T>
    constexpr bool IsA(auto&& entity)
    {
        return std::is_base_of_v<T, std::remove_cvref_t<decltype(entity)>>;
    }

    void Run()
    {
        Johnson j{};
        std::println("{}", IsA<Human>(j));
    }
}

export namespace TagsTesting
{
    template<size_t N>
    struct FixedString
    {
        char Buffer[N];

        consteval size_t Size() const noexcept { return N; }

        consteval size_t StrLen() const noexcept { return N - 1; }

        constexpr FixedString(const char(&arg)[N]) noexcept
        {
            std::copy_n(arg, N, Buffer);
        }

        constexpr bool operator==(FixedString<N> other) const noexcept
        {
            return std::equal(other.Buffer, other.Buffer + N, Buffer);
        }

        template<size_t M>
        constexpr bool operator==(FixedString<M> other) const noexcept
        {
            return false;
        }

        template<size_t M>
        consteval FixedString<M + N - 1> operator+(FixedString<M> other) const noexcept
        {
            char out[M + N - 1];
            std::copy_n(Buffer, N - 1, out);
            std::copy_n(other.Buffer, M, out + N - 1);
            return out;
        }
    };

    template<FixedString...TStrings>
    struct TagsCollection
    {
        template<size_t N>
        constexpr bool Has(FixedString<N> test) const noexcept
        {
            return
                []<typename...TStrings>(FixedString<N> op, TStrings...input)
                {
                    return (
                        []<size_t M>(FixedString<N> test, FixedString<M> val)
                        {
                            return test == val;
                        }(op, input) or ...);
                }(test, TStrings...);
        }

        template<size_t N>
        constexpr bool Has(const char(&arg)[N]) const noexcept
        {
            return Has(FixedString<N>(arg));
        }
    };

    struct P
    {
        TagsCollection<"a"> Collection;
    };

    void Run()
    {
        TagsCollection<"a"> F;

        TagsCollection<"A", "B"> H;

        std::println("{}", H.Has("A"));

        constexpr FixedString A{ "a" };
        constexpr FixedString B{ "B" };
        constexpr FixedString C = A + B;
    }
}

namespace ImplicitLifetimes
{
    template<typename T>
    void F()
    {
        constexpr bool HasCount = requires(T t) { { t.count() } -> std::convertible_to<int>; };
        if constexpr (HasCount)
        {

        }
    }

    // https://en.cppreference.com/w/cpp/memory/start_lifetime_as
    void Run()
    {
        alignas(std::complex<float>) unsigned char network_data[sizeof(std::complex<float>)]
        {
            0xcd, 0xcc, 0xcc, 0x3d, 0xcd, 0xcc, 0x4c, 0x3e
        };

        //  auto d = *reinterpret_cast<std::complex<float>*>(network_data);
        //  std::cout << d << '\n'; // UB: network_data does not point to a complex<float>

        //  auto d1 = *std::launder(reinterpret_cast<std::complex<float>*>(network_data));
        //  std::cout << d1 << '\n'; // UB: implicitly created objects have dynamic storage
        //                                  duration and have indeterminate value initially,
        //                                  even when an array which provides storage for
        //                                  them has determinate bytes.
        //                                  See also CWG2721.

        // Not currently implemented by MSVC STL
        //auto d2 = *std::start_lifetime_as<std::complex<float>>(network_data);
        //std::cout << d2 << '\n'; // OK
        
    }
}

namespace Termination
{
    // See https://www.studyplan.dev/pro-cpp/rethrowing-exceptions
    // https://github.com/MicrosoftDocs/cpp-docs/blob/main/docs/cpp/transporting-exceptions-between-threads.md
    void Run()
    {
        constexpr auto FinalException =
            []
            {
                try
                {
                    if (auto ptr = std::current_exception(); ptr)
                        std::rethrow_exception(ptr);
                    std::abort(); // shouldn't hit this line, unexpected
                }
                // catch other types
                catch (const std::exception& ex)
                {
                    std::abort();
                }
                catch (...)
                {
                    std::abort();
                }
            };

        // Unwinds standard nested exceptions
        constexpr auto NestedException =
            [](this auto self, const std::exception& ex) -> void // trailing return type required because MSVC
            {
                try
                {
                    std::rethrow_if_nested(ex);
                    FinalException();
                }
                catch (const std::exception& ex)
                {
                    self(ex);
                }
                catch (...)
                {
                    std::abort();
                }
            };

        std::set_terminate(
            []
            {
                std::exception_ptr ptr = std::current_exception();
                if (not ptr) // no exception, means terminate() was manually called
                    std::abort();

                try
                {
                    std::rethrow_exception(ptr);
                }
                catch (const std::exception& ex)
                {
                    NestedException(ex);
                }
                catch (...)
                {
                    std::abort();
                }
            });
    }
}
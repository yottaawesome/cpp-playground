#include <windows.h>
import std;
import std.compat;

namespace Timing
{
    namespace CheckDuration
    {
        // Adapted from https://stackoverflow.com/a/41851068/7448661
        template<class T>
        struct is_duration : std::false_type {};

        template<class Rep, class Period>
        struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type {};

        template<typename T>
        concept Duration = is_duration<T>::value;

        void SomeFunctionThatAcceptsDurations(Duration auto s)
        {
            static_assert(is_duration<decltype(s)>::value);
        }

        void Check()
        {
            SomeFunctionThatAcceptsDurations(std::chrono::milliseconds(2));
        }
    }

    void TimeFunctionCallStd()
    {
        auto someLambda = 
            []{
                for (int i = 0; i < 10; i++)
                    std::cout << std::format("{}\n", i);
            };
        const auto begin = std::chrono::high_resolution_clock::now();
        someLambda();
        const auto end = std::chrono::high_resolution_clock::now();
        const auto diff = end - begin;
        std::cout << std::format(
            "{} microseconds = {} nanoseconds\n", 
            std::chrono::duration_cast<std::chrono::microseconds>(diff).count(),
            std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count()
        );
    }

    void TimeFunctionCallsWindows()
    {
        LARGE_INTEGER l1{ 0 };
        LARGE_INTEGER l2{ 0 };
        QueryPerformanceFrequency(&l1);
        uint64_t frequency = l1.QuadPart;

        QueryPerformanceCounter(&l1);
        Sleep(1000);
        QueryPerformanceCounter(&l2);
        uint64_t firstPoint = l1.QuadPart;
        uint64_t secondPoint = l2.QuadPart;
        uint64_t difference = secondPoint - firstPoint;
        std::cout << std::format("{} seconds\n", (double)difference / frequency);
    }

    void NanoSecondIntervals()
    {
        using HundredNanoSecondIntervals = std::chrono::duration<long long, std::ratio<1, 10'000'000>>;
        std::chrono::seconds twoSeconds(2);
        HundredNanoSecondIntervals converted = std::chrono::duration_cast<HundredNanoSecondIntervals>(twoSeconds);
        std::cout << std::format("{} 100-ns intervals in {} seconds", converted.count(), twoSeconds.count());
    }

    template <size_t N>
    struct FixedString
    {
        char buf[N]{};
        //consteval FixedString(char const* arg) noexcept
        consteval FixedString(const char(&arg)[N]) noexcept
        {
            for (unsigned i = 0; i < N; i++)
                buf[i] = arg[i];
            //std::copy_n(arg, N, buf);
        }

        constexpr operator const char* () const noexcept // returns a random pointer when consteval, probable msvc bug
        {
            return buf;
        }

        consteval size_t Size() const noexcept { return N; }

        consteval bool Empty() const noexcept
        {
            return N == 1;
        }

        constexpr const char* Data() const noexcept
        {
            return buf;
        }

        consteval std::string_view View() const noexcept { return { buf, N }; }
    };
    template<size_t N>
    FixedString(char const (&)[N]) -> FixedString<N>;
    //template<size_t N>
    //FixedString(char const (&)[N]) -> FixedString<N-1>;

    template<auto T, bool Enabled = true, bool ThrowErrors = true, FixedString FSComment = "">
    struct TimedScope
    {
        inline void operator()(auto&&...args)
            requires ThrowErrors
        {
            if constexpr (Enabled)
            {
                const auto begin = std::chrono::high_resolution_clock::now();
                T(std::forward<decltype(args)>(args)...);
                const auto end = std::chrono::high_resolution_clock::now();
                diff = end - begin;
                if (not FSComment.Empty())
                {
                    std::cout << std::format(FSComment.Data(), ToMicroseconds().count());
                }
            }
            else
            {
                T(std::forward<decltype(args)>(args)...);
            }
        }

        inline void operator()(auto&&...args) noexcept(Enabled) // don't inadvertently swallow exceptions when test is disabled
            requires not ThrowErrors
        {
            if constexpr (Enabled)
            {
                std::chrono::high_resolution_clock::time_point end;
                const auto begin = std::chrono::high_resolution_clock::now();
                try
                {
                    T(std::forward<decltype(args)>(args)...);
                    end = std::chrono::high_resolution_clock::now();
                }
                catch (...)
                {
                    end = std::chrono::high_resolution_clock::now();
                }
                diff = end - begin;
                if (not FSComment.Empty())
                {
                    std::string_view v{ FSComment.Data(), FSComment.Size() };
                    std::cout << std::format("{}, Microseconds: {}\n", (const char*)FSComment, ToMicroseconds().count());
                }
            }
            else
            {
                T(std::forward<decltype(args)>(args)...);
            }
        }

        inline std::chrono::high_resolution_clock::duration Get() const noexcept
        {
            return diff;
        }

        inline std::chrono::microseconds ToMicroseconds() const noexcept
        {
            return std::chrono::duration_cast<std::chrono::microseconds>(diff);
        }

        inline std::chrono::nanoseconds ToNanoseconds() const noexcept
        {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(diff);
        }

        template<typename TDuration>
        inline TDuration To() const noexcept
        {
            return std::chrono::duration_cast<TDuration>(diff);
        }

        private:
            std::chrono::high_resolution_clock::duration diff;
    };

    void SomeFunc() {}
    using Blah = TimedScope<SomeFunc>; // with C++23's static operator(), it may be possible to alias functions this way
    Blah X;

    void TestTimedScope()
    {
        SYSTEMTIME st{ 0 };
        TimedScope<GetSystemTime> ts;
        ts(&st);
        std::cout << std::format(
            "Call took {} microseconds = {} nanoseconds\n",
            ts.ToMicroseconds().count(),
            ts.ToNanoseconds().count()
        );
    }

    void TestTimedScopeLambda()
    {
        constexpr auto func = []
            {
                SYSTEMTIME st{ 0 };
                GetSystemTime(&st);
            };
        
        TimedScope<func> ts;
        ts();
        std::cout << std::format(
            "Call took {} microseconds = {} nanoseconds\n",
            ts.ToMicroseconds().count(),
            ts.ToNanoseconds().count()
        );
    }

    void TestTimedScopeThrowingLambda()
    {
        constexpr auto func = 
            []{
                throw std::runtime_error("Some error");
            };

        TimedScope<func, true, false, "Throwing call is"> ts;
        ts();
        std::cout << std::format(
            "Throwing call took {} microseconds = {} nanoseconds\n",
            ts.ToMicroseconds().count(),
            ts.ToNanoseconds().count()
        );
    }
}

int main()
{
    //Timing::X();
    //Timing::TestTimedScope();
    //Timing::TestTimedScopeLambda();
    Timing::TestTimedScopeThrowingLambda();
    //Timing::TimeFunctionCallStd();
    //Timing::NanoSecondIntervals();
    //Timing::TimeFunctionCallsWindows();

    return 0;
}

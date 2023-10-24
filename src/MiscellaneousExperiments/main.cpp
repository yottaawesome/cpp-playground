#include <windows.h>
import std;
import std.compat;

namespace Timing
{
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

    template<auto T, bool ThrowErrors = true>
    struct TimedScope
    {
        inline void operator()(auto&&...args)
            requires ThrowErrors
        {
            const auto begin = std::chrono::high_resolution_clock::now();
            T(std::forward<decltype(args)>(args)...);
            const auto end = std::chrono::high_resolution_clock::now();
            diff = end - begin;
        }

        inline void operator()(auto&&...args) noexcept
            requires not ThrowErrors
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

        private:
            std::chrono::high_resolution_clock::duration diff;
    };

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
}

int main()
{
    Timing::TestTimedScope();
    Timing::TestTimedScopeLambda();
    //Timing::TimeFunctionCallStd();
    //Timing::NanoSecondIntervals();
    //Timing::TimeFunctionCallsWindows();

    return 0;
}

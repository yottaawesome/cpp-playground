import std;
import coroutines;

namespace StaticTests
{
    constexpr auto IsPercent(std::string_view toTest) -> bool
    {
        if (not toTest.ends_with('%'))
            return false;
        constexpr auto IsDigit =
            [](auto c) static { return c >= '0' and c <= '9'; };

        std::string_view numberPart{ toTest.data(), toTest.length() - 1 };
        if (numberPart.ends_with('.'))
            return false;

        bool decimalPointSeen = false;
        for (char c : numberPart)
        {
            if (IsDigit(c))
                continue;
            if (c != '.' or decimalPointSeen)
                return false;
            decimalPointSeen = true;
        }
        return true;
    }
    static_assert(IsPercent("100%"), "Test failed");
    static_assert(IsPercent("100.0%"), "Test failed");
    static_assert(not IsPercent("100"), "Test failed");
    static_assert(not IsPercent("100.%"), "Test failed");
    static_assert(not IsPercent("9o0%"), "Test failed");
}

int main()
{
    MoreCoro::Run();
    //ExceptionTest::Run();
    //Coroutines::ThreadSwitch::RunSample();
    //Coroutines::Generator::RunSample();
    //Coroutines::EagerFuture::RunSample();
    //Coroutines::LazyFuture::RunSample();
    //Coroutines::ManualResetEvent::Run();
    //Coroutines::Simplest::Run();
    //Coroutines::EvenMoreBasicAwaitables::Run();
    //Coroutines::Random::Run();
    //Coroutines::WithFutex::Run();
    //Coroutines::EvenMoreBasicAwaitables::Run();
    //Signal::Run();
    //Win32Event::Run();
    //Coroutines::Communication::Run();
    //Coroutines::Communication3::Run();
    return 0;
}
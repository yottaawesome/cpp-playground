import std;
import coroutines;










//{
//    if constexpr (std::same_as<std::string&&, decltype(ref)>)
//    {
//        return std::string{ ref };
//    }
//    else if constexpr (std::same_as<std::string&, decltype(ref)>)
//    {
//        return ref;
//    }
//    else if constexpr (std::same_as<const std::string&, decltype(ref)>)
//    {
//        return ref;
//    }
//    else
//    {
//        return 1;
//    }
//}

decltype(auto) Get(auto&& ref)
{
    if constexpr (std::same_as<std::string&&, decltype(ref)>)
    {
        return std::string{ ref };
    }
    else if constexpr (std::same_as<std::string&, decltype(ref)>)
    {
        return ref;
    }
    else if constexpr (std::same_as<const std::string&, decltype(ref)>)
    {
        return ref;
    }
    else
    {
        return 1;
    }

}

const int& t(const int& m) { return m; }

int main()
{
    if constexpr (std::same_as<decltype(Get(std::string{})), std::string>)
    {

    }

    const int& g = t(1);


    //Coroutines::ThreadSwitch::RunSample();
    //Coroutines::Generator::RunSample();
    //Coroutines::EagerFuture::RunSample();
    //Coroutines::Random::Run();
    Coroutines::WithFutex::Run();
    return 0;
}
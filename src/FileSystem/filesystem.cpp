module filesystem;
import std;

template<typename...TArgs, std::invocable<TArgs...> TFunc>
auto FuncInvoke(TFunc&& f, TArgs&&...args)
{
    std::invoke(f, std::forward<TArgs>(args)...);
}

template<typename T>
struct is_expected : std::false_type {};

template<typename T, typename S>
struct is_expected<std::expected<T, S>> : std::true_type {};

template<typename T>
constexpr bool is_expected_v = is_expected<T>::value;

template<typename T>
concept Expected = is_expected_v<T>;

namespace Random
{
    void Run()
    {
        FuncInvoke([](int i) { return i; }, 1);

        std::expected result =
            []<typename TFunc>(this auto && self, TFunc && func, int attempts)
                -> std::expected<std::invoke_result_t<TFunc>, std::string>
            {
                try
                {
                    if constexpr (std::same_as<std::invoke_result_t<TFunc>, void>)
                    {
                        std::invoke(func);
                        return {};
                    }
                    else
                        return std::invoke(func);
                }
                catch (const std::exception& ex)
                {
                    if (--attempts == 0)
                        return std::unexpected(ex.what());
                    return self(func, attempts);
                }
            }([] {}, 5);

        std::expected result2 =
            []<typename...TArgs>(this auto && self, int attempts, auto && func, TArgs&&...args)
                -> std::expected<void, std::string>
            {
                try
                {
                    std::invoke(func, std::forward<TArgs>(args)...);
                    return {};
                }
                catch (const std::exception& ex)
                {
                    if (--attempts == 0)
                        return std::unexpected(ex.what());
                    return self(attempts, func, std::forward<TArgs>(args)...);
                }
            }(5, [](const std::string& s) {}, std::string{ "aaa" });
    }
}
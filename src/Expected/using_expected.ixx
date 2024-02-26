export module using_expected;
import std;

export namespace using_expected
{
    struct test_struct
    {
        int some_value = [] { return 1; }();

        static std::expected<test_struct, bool> try_success() noexcept try
        {
            return test_struct{};
        }
        catch (...)
        {
            return std::unexpected(false);
        }

        static std::expected<test_struct, bool> try_fail() noexcept try
        {
            throw std::runtime_error("Failed!");
        }
        catch (...)
        {
            return std::unexpected(false);
        }
    };

    void Run()
    {
        auto result = test_struct::try_success();
        if (result)
            std::println("Have value");
        result.and_then(
            [](const test_struct& t)
            {
                return std::expected<int, bool>(t.some_value);
            });
        // transforms from std::expected<test_struct, bool> to std::expected<string, bool> 
        result.transform(
            [](const test_struct& t)
            {
                return std::string{};
            });

        result = test_struct::try_fail();
        if (not result)
            std::println("Failed to get value");

        // if unexpected, sets the value to the parameter
        result.value_or(test_struct{});
        result.error_or(false);
        // transforms from std::expected<..., bool> to std::expected<..., string> 
        result.transform_error(
            [](bool b)
            {
                return std::string{};
            });

        if (not result)
            std::println("Failed to get value");
    }
}
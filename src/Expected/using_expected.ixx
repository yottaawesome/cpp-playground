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

    void run()
    {
        auto result = test_struct::try_success();
        if (result)
            std::println("Have value");
        result.and_then(
            [](const test_struct& t) -> std::expected<int, bool> // can change the expected's primary type
            {
                return t.some_value;
            });
        result.or_else(
            [](bool s) -> std::expected<test_struct, std::string> // can change the expected's error type
            {
                return test_struct{};
            });

        result.transform( // transforms from std::expected<test_struct, bool> to std::expected<string, int> 
            [](const test_struct& t) -> std::expected<std::string, int>
            {
                return std::string{};
            });
        
        result 
            .transform(
                [](const test_struct& t)
                {
                    return std::string{};
                })
            .or_else(
                [](bool s)
                {
                    return std::expected<std::string, bool>{{}};
                })
            .value_or(std::string{ "blah" });

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

export namespace using_expected_2
{
    struct test
    {
        int field_a = 0;
        int field_b = 1;
        int field_c = 2;
        int field_d = 3;

        static std::expected<test, bool> try_get() noexcept try
        {
            return test{};
        }
        catch (...)
        {
            return std::unexpected(false);
        }
    };

    void run()
    {
        auto [a, b] = test::try_get()
            .transform([](const test& t) { return std::make_tuple(t.field_a, t.field_c); })
            .value_or(std::make_tuple(0, 0)) // use this
            //.or_else( // or this
            //    [](bool b)
            //    {
            //        return std::expected<std::tuple<int, int>, bool>{std::make_tuple(0, 0)};
            //    })
            //.value()
            ;
    }
}
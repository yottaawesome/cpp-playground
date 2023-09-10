import std;

namespace ApproachA
{
    struct ExactString
    {
        std::string Message;
    };

    class Ex1 : public std::exception
    {
        public:
            virtual ~Ex1() = default;

            Ex1(std::string_view message)
                : std::exception(Generate(message).c_str()) {}

            Ex1(const ExactString& exact)
                : std::exception(exact.Message.c_str()) {}

        private:
            std::string Generate(std::string_view message) const
            {
                return std::format("This is {}", message);
            }
    };

    class Ex2 : public Ex1
    {
        public:
            virtual ~Ex2() = default;

            Ex2(std::string message)
                : Ex1(Generate(message)) {}

        private:
            ExactString Generate(std::string_view message) const
            {
                return { std::format("This is Ex2::Generate(): {}", message) };
            }
    };
}

namespace ApproachB
{
    class Ex1 : public std::exception
    {
        public:
            virtual ~Ex1() = default;

            Ex1(std::string_view message)
                : std::exception(Generate(message).c_str()) {}

            Ex1(
                std::string_view message,
                const auto& generator,
                auto&&...args
            ) : std::exception(generator(message, std::forward<decltype(args)>(args)...).c_str())
            {}

        private:
            std::string Generate(std::string_view message) const
            {
                return std::format("This is {}", message);
            }
    };

    class Ex2 : public Ex1
    {
        public:
            virtual ~Ex2() = default;

            Ex2(std::string_view message, bool second)
                : Ex1(message, GenerateOther2, false) {}

        private:
            static std::string GenerateOther1(std::string_view message)
            {
                return std::format("This is Ex2::Generate(): {}", message);
            }

            static std::string GenerateOther2(std::string_view message, const bool b)
            {
                return std::format("This is Ex2::Generate(): {} {}", message, b);
            }
    };
}



int main()
{
    ApproachB::Ex2 ex("Some error", true);
    std::cout << ex.what() << std::endl;

    return 0;
}

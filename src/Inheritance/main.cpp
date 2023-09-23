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

namespace ApproachC
{
    template<typename G = void, typename...T>
    class Ex1 : public virtual std::exception, virtual public T...
    {
        public:
            using std::exception::what;

            Ex1() requires (sizeof...(T) == 0)
                : T()...
            { }

            Ex1(const std::string_view message)
                requires std::is_same_v<G, void>
            : std::exception(message.data())
            { }

            Ex1(const std::string_view message, const auto&&...args)
                requires !std::is_same_v<G, void>
                : std::exception(
                    G::GenerateErrorMessage(
                        message, 
                        std::forward<decltype(args)>(args)...
                    ).c_str()
                )
            { }
    };

    struct WinErrorGenerator
    {
        static std::string GenerateErrorMessage(const std::string_view message)
        {
            return std::format("WinError: {}", message);
        }
    };
    template<typename T = WinErrorGenerator>
    using WinError = Ex1<T>;
    
    struct WinErrorGenerator2
    {
        static std::string GenerateErrorMessage(const std::string_view message)
        {
            return std::format("WinError2: {}", message);
        }
    };
    using WinError2 = Ex1<WinErrorGenerator2, WinError<WinErrorGenerator>>;

    struct WinErrorGenerator3
    {
        static std::string GenerateErrorMessage(const std::string_view message)
        {
            return std::format("WinError3: {}", message);
        }
    };
    using WinError3 = WinError<WinErrorGenerator3>;

}

namespace ApproachD
{
    struct ExactMessage
    {
        std::string Message;
    };
    
    template<typename TGenerator, typename TParent>
    concept HasNoGenerator = std::is_same_v<TGenerator, void>;

    template<typename TGenerator, typename TParent>
    concept HasGeneratorStdExceptionIsParent =
        (!std::is_same_v<TGenerator, void> && std::is_same_v<TParent, std::exception>);

    template<typename TGenerator, typename TParent>
    concept HasGeneratorStdExceptionIsNotParent =
        (!std::is_same_v<TGenerator, void> && !std::is_same_v<TParent, std::exception>);

    template<typename TGenerator, typename...Args>
    concept GeneratorSignature = requires(Args...args)
    {
        {TGenerator::Generate("something", std::forward<Args>(args)...).Message} -> std::convertible_to<std::string>;
    };

    template<typename TGenerator = void, typename TParent = std::exception>
    class Ex1 : public TParent
    {
        public:
            virtual ~Ex1() = default;

            Ex1(const std::string_view message)
                requires HasNoGenerator
            : TParent(message.data())
            { }

            // Only defined for types whose immediate parent is std::exception
            Ex1(const std::string_view message, const auto&&...args)
                requires HasGeneratorStdExceptionIsParent<TGenerator, TParent>
            : TParent(
                TGenerator::Generate(
                    message,
                    std::forward<decltype(args)>(args)...
                ).c_str()
            )
            { }

            // Only defined for types whose immediate parent is NOT std::exception
            template<typename...Args>
            Ex1(
                const std::string_view message, 
                const Args&&...args
            ) requires 
                (HasGeneratorStdExceptionIsNotParent<TGenerator, TParent> 
                && GeneratorSignature<TGenerator, Args...>)
            : TParent(
                TGenerator::Generate(
                    message,
                    std::forward<Args>(args)...
                )
            )
            { }
    
        protected:
            Ex1(ExactMessage message)
                : std::exception(message.Message.c_str())
            { }
    };

    struct WinErrorGenerator
    {
        static std::string Generate(const std::string_view message)
        {
            return std::format("WinError: {}", message);
        }
    };
    //template<typename T = WinErrorGenerator>
    //using WinError = Ex1<T>;

    using WinError = Ex1<WinErrorGenerator>;

    struct WinErrorGenerator2
    {
        static ExactMessage Generate(const std::string_view message)
        {
            return ExactMessage{ std::format("WinError2 exact: {}", message) };
        }
    };
    using WinError2 = Ex1<WinErrorGenerator2, WinError>;
}

namespace ApproachE
{
    template <typename Derived, typename TImpl = Derived>
    class BaseError : public std::exception
    {
        public:
            BaseError(std::string_view msg) : std::exception(Generate(msg).c_str()) {}

        protected:
            std::string Generate(std::string_view msg)
            {
                return static_cast<TImpl*>(this)->Implementation(msg);
            }

            std::string Implementation(std::string_view msg)
            {
                return "Implementation Base";
            }
    };

    class WinError : public BaseError<WinError>
    {
        public:
            WinError(std::string_view msg) : BaseError<WinError>(msg) {}

            std::string Implementation(std::string_view msg)
            {
                return "Implementation Derived1";
            }
    };

    class WinError2 : public BaseError<WinError, WinError2>
    {
        public:
            WinError2(std::string_view msg) : BaseError<WinError, WinError2>(msg) {}

            std::string Implementation(std::string_view msg)
            {
                return "Implementation Derived2";
            }
    };
}

struct Gen
{
    void Generate() {};
    static void Generate2() {};
};

template<typename...Args>
void DoesSignatureMatch(Args&&...args)
{
    auto x = (std::get<sizeof...(args) - 1>(std::tuple{ args... }));
    decltype(x)::Generate2();
    x.Generate();
}

int main()
{
    ApproachB::Ex2 ex("Some error", true);

    ApproachC::Ex1 x("A");
    ApproachC::WinError win("Blah blah");

    std::cout << win.what() << std::endl;

    ApproachC::WinError2 win2("Blah blah");
    std::cout << win2.what() << std::endl;

    ApproachD::WinError win3("Blah blah");
    std::cout << win3.what() << std::endl;

    ApproachD::WinError2 win4("Blah blah");
    std::cout << win4.what() << std::endl;

    std::exception win5 = win4;
    std::cout << win5.what() << std::endl;

    ApproachE::WinError win6("blah blah");
    std::cout << win6.what() << std::endl;

    ApproachE::WinError2 win7("blah blah");
    std::cout << win7.what() << std::endl;

    try
    {
        throw ApproachD::WinError2("blah blah");
    }
    catch (const ApproachD::WinError& err)
    {
        std::cout << err.what() << std::endl;
    }

    
    Gen g;
    DoesSignatureMatch(g);

    return 0;
}

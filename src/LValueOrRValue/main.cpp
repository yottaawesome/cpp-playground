import std;

namespace TestA
{
    // based on https://www.fluentcpp.com/2022/05/16/how-to-store-an-lvalue-or-an-rvalue-in-the-same-object/
    template<typename T>
    struct NonConstReference
    {
        T& value_;
        explicit NonConstReference(T& value) : value_(value) {};
    };

    template<typename T>
    struct ConstReference
    {
        T const& value_;
        explicit ConstReference(T const& value) : value_(value) {};
    };

    template<typename T>
    struct Value
    {
        T value_;
        explicit Value(T&& value) : value_(std::move(value)) {}
    };

    template<typename T>
    using Storage = std::variant<Value<T>, ConstReference<T>, NonConstReference<T>>;

    template<typename... Functions>
    struct overload : Functions...
    {
        using Functions::operator()...;
        overload(Functions... functions) : Functions(functions)... {}
    };

    // Defining const access
    template<typename T>
    T const& getConstReference(Storage<T> const& storage)
    {
        return std::visit(
            overload(
                [](Value<T> const& value) -> T const& { return value.value_; },
                [](NonConstReference<T> const& value) -> T const& { return value.value_; },
                [](ConstReference<T> const& value) -> T const& { return value.value_; }
            ),
            storage
        );
    }

    // Defining non-const access
    struct NonConstReferenceFromReference : public std::runtime_error
    {
        explicit NonConstReferenceFromReference(std::string const& what) : std::runtime_error{ what } {}
    };

    template<typename T>
    T& getReference(Storage<T>& storage)
    {
        return std::visit(
            overload(
                [](Value<T>& value) -> T& { return value.value_; },
                [](NonConstReference<T>& value) -> T& { return value.value_; },
                [](ConstReference<T>&) -> T& { throw NonConstReferenceFromReference{ "Cannot get a non const reference from a const reference" }; }
            ),
            storage
        );
    }

    class MyClass
    {
        public:
            explicit MyClass(std::string& value) : storage_(NonConstReference(value)) {}
            explicit MyClass(std::string const& value) : storage_(ConstReference(value)) {}
            explicit MyClass(std::string&& value) : storage_(Value(std::move(value))) {}

            void print() const
            {
                std::cout << getConstReference(storage_) << '\n';
            }

        private:
            Storage<std::string> storage_;
    };

    void Run()
    {
        std::string s = "hello";
        MyClass myObject1{ s };
        myObject1.print();

        MyClass myObject2{ std::string{"hello"} };
        myObject2.print();
    }
}

namespace TestB
{
    template<typename T>
    struct CopyOrMove
    {
        CopyOrMove() 
            requires std::is_default_constructible_v<T>
            : m_value(T()) 
        {}
        CopyOrMove(const T& value) 
            requires std::is_copy_constructible_v<T>
            : m_value(value), m_movable(false)
        {}
        CopyOrMove(T& value) 
            requires std::is_copy_constructible_v<T>
            : m_value(value), m_movable(false)
        {}
        CopyOrMove(T&& value)
            requires std::is_move_constructible_v<T>
            : m_value(value), m_movable(true)
        {}

        //template<typename...Args>
        //CopyOrMove(Args...args) : m_value(T(std::forward<Args...>(args)...)) {}

        T& m_value;
        bool m_movable;

        auto Unwrap() noexcept
        {
            if (m_movable)
                return std::move(m_value);
            return m_value;
        }

        void Assign(T& other)
        {
            other = m_movable ? std::move(m_value) : m_value;
        }
    };
    //template<typename T>
    //CopyOrMove(const char*) -> CopyOrMove<std::string>;

    void SomeFunction(CopyOrMove<std::string> s)
    {
        std::string a;
        a = s.Unwrap();
        std::cout << a;
    }

    class SomeClass
    {
        public:
            SomeClass(CopyOrMove<std::string> str)
                : m_str(str.Unwrap())
            {}

        private:
            std::string m_str;
    };

    void Run()
    {
        //std::string a{ "aaa" };
        //SomeFunction({ "aaa" });
        SomeClass b({ "aaa" });
    }
}

int main()
{
    TestA::Run();
    TestB::Run();
    return 0;
}

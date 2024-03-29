// See https://www.youtube.com/watch?v=gTNJXVmuRRA&list=PLHTh1InhhwT6c2JNtUiJkaH8YRqzhU7Ag&index=7
// https://github.com/CppCon/CppCon2022/blob/main/Presentations/Using-Modern-C-to-Eliminate-Virtual-Functions-Jonathan-Gopel-CppCon-2022.pdf

import std;
import std.compat;

namespace ConceptFooleryA
{
    template<auto V>
    struct S : std::false_type {};

    template<>
    struct S<1> : std::true_type {};

    template<auto T>
    concept IsS = S<T>::value and T == 1; // deliberately redundant

    template<typename T>
    concept IsS2 = requires(T t)
    {
        true;
    };

    template<auto T>
    concept IsS3 = requires() { true; };

    template<typename T>
    concept SomeTest = std::is_null_pointer_v<T> or std::is_invocable_v<T>;

    template<SomeTest auto L = nullptr>
    struct Moo {};

    template<IsS2 auto V>
        requires IsS<V>
    void SomeFunc() { }

    void Test()
    {
        SomeFunc<1>();

        Moo<nullptr> x;
        Moo<[]{}> y;
    }
}

namespace ConceptFooleryB
{
    class A { };

    void GH() noexcept {}

    // https://stackoverflow.com/questions/44269678/stdis-nothrow-invocable-with-member-function
    // https://mariusbancila.ro/blog/2022/06/20/requires-expressions-and-requires-clauses-in-cpp20/
    // https://www.fluentcpp.com/2020/09/11/replacing-crtp-static-polymorphism-with-concepts/
    template<typename T>
    concept No = noexcept(std::declval<T>().Another());

    template<typename T>
    concept No2 = noexcept(T());

    template<typename T>
    concept IInterface = No<T> && requires(T t, const T m, const A a)
    {
        typename T::IsType;
        {t.Blah()} -> std::same_as<int>;
        {t.AnotherBlah()} -> std::same_as<std::string>;
        //{std::declval<const T&>().AnotherBlah()} -> std::same_as<std::string>;
        {m.AnotherBlah()} -> std::same_as<std::string>; // Same as above
        {t.Something(a)} -> std::same_as<void>;
        {m.Another3(1)} noexcept -> std::same_as<void>;
        //{noexcept(std::declval<T>().Another2())} -> true;
            requires noexcept(t.Another());
            requires noexcept(std::declval<T>().Another2(1));
            requires std::is_nothrow_invocable_v<decltype(&T::Another2), T, int>;
    };

    template<typename T>
    constexpr bool DoesSignatureMatch(auto func)
    {
        return std::same_as<decltype(func), T>;
    }

    template<typename T>
    concept IInterface2 = requires(T t)
    {
        // Exact match on argument parameters, whereas {t.Another3(long long{})} -> ...
        // will pass on functions with narrowing narrower types
        requires std::same_as<decltype(&T::Another3), void(T::*)(int)const noexcept>;
        requires std::same_as<decltype(&T::Another3), auto(T::*)(int)const noexcept -> void>;
        requires DoesSignatureMatch<void(T::*)(int)const noexcept>(&T::Another3);
    };

    template <typename T>
    constexpr bool always_false = std::false_type::value;

    template<typename T>
    void SomeTest(T t)
    {
        if constexpr (std::same_as<T, int>)
        {

        }
        else
        {
            // https://mariusbancila.ro/blog/2022/06/20/requires-expressions-and-requires-clauses-in-cpp20/
            static_assert(always_false<T>, "Nah");
        }
    }

    class SomeClass
    {
        public:
            using IsType = int;
            int Blah() { return 1; }
            std::string AnotherBlah() const { return "Blah"; }
            void Something(A i) {}
            void Another() noexcept { }
            void Another2(int i) noexcept { }
            void Another3(int i) const noexcept { }
    };

    static_assert(IInterface<SomeClass>, "Must conform to the requirements of the IInterface concept");
    static_assert(IInterface2<SomeClass>, "Must conform to the requirements of the IInterface concept");

    void Func(IInterface auto& obj)
    {
        obj.Blah();
    }

    void Func2(IInterface2 auto& obj)
    {
        obj.Another3();
    }

    template<typename T, typename... Ts>
    concept same_as_any = (... or std::same_as<T, Ts>);

    template<IInterface... TFoos>
    class Bar
    {
        public:
            constexpr Bar(same_as_any<TFoos...> auto input)
                : foo(input)
            { }

            constexpr void set_foo(same_as_any<TFoos...> auto input)
            {
                foo = input;
            }

    private:
        std::variant<TFoos...> foo{};
    };

    template<IInterface TFoos>
    class Bar2
    {
        public:
            constexpr Bar2(IInterface auto input)
                : foo(input)
            { }

            constexpr void set_foo(IInterface auto input)
            {
                foo = input;
            }

        private:
            TFoos foo{};
    };

    template<IInterface...TFoos>
    class Baz
    {
        public:
            auto store(same_as_any<TFoos...> auto value)
            {
                return std::get<std::vector<decltype(value)>>(data).push_back(value);
            }

            template<same_as_any<TFoos...> T>
            auto store2(T value)
            {
                return std::get<std::vector<T>>(data).push_back(value);
            }

        private:
            std::tuple<std::vector<TFoos>...> data{};
    };

    void Test()
    {
        SomeTest(1);
        SomeClass c;
        Func(c);

        if (true xor false)
        {

        }

        Bar<SomeClass> bar(c);
        static_assert(No<SomeClass>);
    }
}

int main()
{
    ConceptFooleryB::Test();
    return 0;
}


// See https://www.youtube.com/watch?v=gTNJXVmuRRA&list=PLHTh1InhhwT6c2JNtUiJkaH8YRqzhU7Ag&index=7
// https://github.com/CppCon/CppCon2022/blob/main/Presentations/Using-Modern-C-to-Eliminate-Virtual-Functions-Jonathan-Gopel-CppCon-2022.pdf

import std;

class A
{

};

void GH() noexcept {}

// https://stackoverflow.com/questions/44269678/stdis-nothrow-invocable-with-member-function
// https://mariusbancila.ro/blog/2022/06/20/requires-expressions-and-requires-clauses-in-cpp20/
template<typename T>
concept No = noexcept(std::declval<T>().Another());

template<typename T>
concept No2 = noexcept(T());

template<typename T>
concept IInterface = No<T> && requires(T t, const T m, const A a)
{
    {t.Blah()} -> std::same_as<int>;
    {t.AnotherBlah()} -> std::same_as<std::string>;
    //{std::declval<const T&>().AnotherBlah()} -> std::same_as<std::string>;
    {m.AnotherBlah()} -> std::same_as<std::string>; // Same as above
    {t.Something(a)} -> std::same_as<void>;
    //{noexcept(std::declval<T>().Another2())} -> true;
    
    //std::is_nothrow_invocable<decltype(&T::Another2), T>;
    
    //noexcept(decltype(t.Another));

    requires std::is_nothrow_invocable_v<decltype(&T::Another2), T, int>;
};


class SomeClass
{
    public:
        int Blah() { return 1; }
        std::string AnotherBlah() const { return "Blah"; }
        void Something(A i) {}
        void Another() noexcept { }
        void Another2(int i) noexcept { }
};

static_assert(IInterface<SomeClass>, "Must conform to the requirements of the IInterface concept");

void Func(IInterface auto& obj)
{
    obj.Blah();
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

int main()
{
    SomeClass c;
    Func(c);

    Bar<SomeClass> bar(c);
    static_assert(No<SomeClass>);

    return 0;
}


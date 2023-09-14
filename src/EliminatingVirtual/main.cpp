// See https://www.youtube.com/watch?v=gTNJXVmuRRA&list=PLHTh1InhhwT6c2JNtUiJkaH8YRqzhU7Ag&index=7
// https://github.com/CppCon/CppCon2022/blob/main/Presentations/Using-Modern-C-to-Eliminate-Virtual-Functions-Jonathan-Gopel-CppCon-2022.pdf

import std;

class A
{

};

template<typename T>
concept IInterface = requires(T t, const T m, const A a)
{
    {t.Blah()} -> std::same_as<int>;
    {t.AnotherBlah()} -> std::same_as<std::string>;
    //{std::declval<const T&>().AnotherBlah()} -> std::same_as<std::string>;
    {m.AnotherBlah()} -> std::same_as<std::string>; // Same as above
    {t.Something(a)} -> std::same_as<void>;
};

class SomeClass
{
    public:
        int Blah() { return 1; }
        std::string AnotherBlah() const { return "Blah"; }
        void Something(A i) {}
};

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

    return 0;
}


import std;
import stringtemplates;

template<typename X>
concept x = requires(X y)
{
    X::What();
    y.What2();
};

struct S
{
    static void What() {}
    void What2() {}
};
static_assert(x<S>);

int main(int argc, char* argv[])
{
    S s;
    return 0;
}

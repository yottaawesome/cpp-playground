import std;

struct Source
{
    auto Get(const std::string_view name)
    {

    }
};

template<typename T>
struct Setting;

template<>
struct Setting<int>
{

};

int main()
{
    Setting<int> f;
    std::cout << "Hello World!\n";
}

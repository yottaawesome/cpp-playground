// Cannot include #include any standard headers for this to work
import std;

int main()
{
    const std::source_location location = std::source_location::current();
    std::cout << std::format("Hello {}!\n", "world");
    return 0;
}

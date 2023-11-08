// Cannot include #include any standard headers for this to work.
// Headers #including the standard headers are probably also going
// to cause this to fail.
import std;
import module1;
import module2;

int main()
{
    const std::source_location location = std::source_location::current();
    std::cout << std::format("Hello {}!\n", "world");
    return 0;
}

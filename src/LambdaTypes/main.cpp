#include <type_traits>
import sample;

int x() { return 1; }

static_assert(std::is_same_v<int, decltype(x())>, "No");

int main() 
{
    A::Test();
    B::Test();
    C::Test();
    return 0;
}
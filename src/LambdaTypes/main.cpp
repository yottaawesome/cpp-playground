import std;
import sample;
import weirdness;

int x() { return 1; }

static_assert(std::is_same_v<int, decltype(x())>, "No");

int main() 
{
    Weirdness::Run();
    A::Test();
    B::Test();
    C::Test();
    D::Test();
    return 0;
}
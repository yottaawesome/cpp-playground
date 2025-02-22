import std;
import coroutines;

int main()
{
    //Coroutines::ThreadSwitch::RunSample();
    //Coroutines::Generator::RunSample();
    //Coroutines::EagerFuture::RunSample();
    Coroutines::LazyFuture::RunSample();
    //Coroutines::Random::Run();
    //Coroutines::WithFutex::Run();
    return 0;
}
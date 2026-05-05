import std;
import std.compat;
import tupletoparameterpack;
import variantmadness;
import moremadness;

template<typename T>
struct Function {};

template<typename Ret, typename...Args>
struct Function<Ret(*)(Args...)>
{
	template<auto Fn>
	struct Invoker
	{
		static inline auto operator()(Args&&... args)
		{
			return std::invoke(Fn, std::forward<Args>(args)...);
		}
	};


	using FnType = Ret(*)(Args...);
	inline auto operator()(this auto& self, Args&&... args)
	{
		//return self.Invoke(std::forward<Args>(args)...);
		//return FnType(std::forward<Args>(args)...);
	}
};

template<auto Fn>
struct Function2 : Function<decltype(Fn)>::template Invoker<Fn>
{
};

template<auto VFn>
auto inline Fn(auto&&... args)
{
	return VFn(std::forward<decltype(args)>(args)...);
}

void SomeFunction(int X, int Y)
{

}

constexpr auto SomeFunctionPtr1 = Function<decltype(&SomeFunction)>{};
constexpr auto SomeFunctionPtr = Function2<SomeFunction>{};

constexpr auto XX = Function<decltype(&SomeFunction)>::Invoker<SomeFunction>{};

int main()
{
	XX(1, 2);
	SomeFunctionPtr(1, 2);
	SomeFunctionPtr(1,2);
	//SomeFunctionPtr.Invoker(5);

	TupleCats::Run();
	Structs::Run();
	PrettyPrint::Run();
	//WaitingB::Run();
	/*TimingPerformance::Run();
	OtherStuff::Run();
	SomeOtherTest::Run();
	MoreStuff::Run();
	MakeSafe::Run();
	LoopOverTypes::Run();
	Chained::Run();
	LoopOverLambdas::Run();
	GetTupleElement::Run();
	TupleFunc::Run();
	SomeTest::Run();
	ReadLineLoop::Run();*/
	
	return 0;
}

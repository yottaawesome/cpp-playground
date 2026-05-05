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
	using FnType = Ret(*)(Args...);
	inline auto operator()(this auto& self, Args&&... args)
	{
		return self.Invoke(std::forward<Args>(args)...);
		//return FnType(std::forward<Args>(args)...);
	}

	inline auto Invoke(this auto&& self, Args&&... args)
	{
		
	}
};

template<auto Fn>
struct Function2 : Function<decltype(Fn)> 
{
	using FnType = Function<decltype(Fn)>::FnType;

	inline auto Invoke(this auto&& self, auto&&... args)
	{
		return Fn(std::forward<decltype(args)>(args)...);
	}

	/*static inline auto operator()(auto&&... args)
	{
		return std::invoke(Fn, std::forward<decltype(args)>(args)...);
	}*/
	//constexpr static auto FnType Invoker = Fn;

};

template<auto VFn>
auto inline Fn(auto&&... args)
{
	return VFn(std::forward<decltype(args)>(args)...);
}

void SomeFunction(int X)
{

}

constexpr auto SomeFunctionPtr1 = Function<decltype(&SomeFunction)>{};
constexpr auto SomeFunctionPtr = Function2<SomeFunction>{};

int main()
{
	SomeFunctionPtr(5);
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

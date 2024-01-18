import std;
import std.compat;
import tupletoparameterpack;
import variantmadness;

std::integral_constant<int, 1> t1;
std::integral_constant<int, 2> t2;

struct V
{
	void operator()(std::integral_constant<int, 1>)
	{
	}

	void operator()(std::integral_constant<int, 2>)
	{
	}
};

template<typename...Args>
auto make(Args&&...args) -> std::tuple<Args...>
{
	return { args... };
}



int main()
{
	MakeSafe::Run();
	//LoopOverTypes::Run();
	//Chained::Run();
	LoopOverLambdas::Run();
	//GetTupleElement::Run();
	//TupleFunc::Run();
	//SomeTest::Run();
	ReadLineLoop::Run();
	
	return 0;
}

import std;
import std.compat;
import tupletoparameterpack;
import variantmadness;

int main()
{
	std::index_sequence<3> arr[3];

	auto x = []<size_t...Is>(std::index_sequence<Is...>)
	{
		return (
			[]<size_t I = Is>()
			{
				if constexpr (I == 1)
				{

				}
			}(), ...
		);
	};

	x(std::make_index_sequence<3>{});

	EvenMoreFunnyStuff::Run();
	return 0;
	OtherStuff::Run();
	SomeOtherTest::Run();
	MoreStuff::Run();
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

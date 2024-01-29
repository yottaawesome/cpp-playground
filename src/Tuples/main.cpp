import std;
import std.compat;
import tupletoparameterpack;
import variantmadness;
import moremadness;

constexpr bool something()
{
	return false;
}

template<bool b = something()>
struct M {};

int main()
{
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

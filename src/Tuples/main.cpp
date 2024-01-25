import std;
import std.compat;
import tupletoparameterpack;
import variantmadness;

int main()
{
	try
	{
		PrettyPrint::Run();
	}
	catch (const std::exception& ex)
	{
		std::println("{}", ex.what());
	}
	//WaitingB::Run();
	TimingPerformance::Run();
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

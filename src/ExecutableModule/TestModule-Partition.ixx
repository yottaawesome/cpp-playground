export module TestModule:Partition;

namespace Y
{
	void M() {}
}

// Anonymous namespace -- visible to only this file
// Not visible anywhere else in the module.
namespace
{
	using namespace Y;
	void BlahBlah() {}

	void BlahBlah2();
}

namespace TestModule::Internal 
{
	namespace
	{

	}
}

namespace TestModule::Internal
{
	using Alias = unsigned long;

	int TestFunc2()
	{
		M();
		BlahBlah();
		BlahBlah2();
		return 1;
	}

	struct TestStruct
	{

	};
}

namespace
{
	void BlahBlah2() {}
}
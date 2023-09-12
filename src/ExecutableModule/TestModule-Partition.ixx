export module TestModule:Partition;

// Anonymous namespace -- visible to only this file
// Not visible anywhere else in the module.
namespace
{
	void BlahBlah() {}

	void BlahBlah2();
}

namespace TestModule::Internal
{
	using Alias = unsigned long;

	int TestFunc2()
	{
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
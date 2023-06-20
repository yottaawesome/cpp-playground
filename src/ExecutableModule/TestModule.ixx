export module TestModule;
export import :Partition;

// See pre-link event in https://stackoverflow.com/questions/2658215/how-do-i-create-both-a-lib-file-and-an-exe-file-in-visual-c
export namespace TestModule 
{
	class Partial;

	Partial* GetPartial();

	int TestFunc()
	{
		return 1;
	}

	// All the following work for exporting non-exported symbols
	// in a partition. Can also prepend export in front of the
	// symbols.
	using Internal::TestStruct;
	using Blah = Internal::TestStruct;
	using Internal::TestFunc2;
	using Internal::Alias;

	// The following do not work.
	// using namespace Internal;
	// using Internal;
}

// This doesn't export the symbols.
export namespace X = TestModule::Internal;

namespace TestModule
{
	// if moved to the private fragment, the class
	// can be instantiated via pointer but not used
	class Partial
	{
	public:
		void Blah() {}
	};
}

module :private;
namespace TestModule
{
	Partial* GetPartial()
	{
		return new Partial();
	}
}
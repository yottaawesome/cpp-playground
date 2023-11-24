module;

#include "Test.hpp"
#include <memory>

export module TestModule;
export import :Partition;
import :InternalPartition;

// See pre-link event in https://stackoverflow.com/questions/2658215/how-do-i-create-both-a-lib-file-and-an-exe-file-in-visual-c
export namespace TestModule 
{
	using TestModule3::SomeClass;
	using ::Unnamespaced;

	class Partial;
	using PtrPartial2 = std::unique_ptr<TestModule2::Partial2>;


	Partial* GetPartial();
	PtrPartial2 GetPartial2();
	std::unique_ptr<TestModule2::Partial2> GetPartial3();

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
	// namespace X = TestModule::Internal;
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

	PtrPartial2 GetPartial2()
	{
		return std::make_unique<TestModule2::Partial2>();
	}

	std::unique_ptr<TestModule2::Partial2> GetPartial3()
	{
		return std::make_unique<TestModule2::Partial2>();
	}
}
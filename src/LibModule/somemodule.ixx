export module somemodule;
import std;

// Export single type.
export struct ClassA {};

// Export block for multiple types.
export
{
	struct ClassB {};
	struct ClassC {};
}

// This used to error with https://developercommunity.visualstudio.com/t/Thread-local-storage-inside-C-module-l/10529078?sort=newest
export struct Singleton
{
	static std::unique_ptr<int>& get() {
		static thread_local std::unique_ptr<int> value;
		return value;
	}
};

consteval auto M()
{

}

// Export an entire namespace.
export namespace NamespaceA
{
	struct ClassD {};
}

// Not exported.
namespace NamespaceB
{
	struct ClassE {};
	void SomeFunction() {}
}

// Export a type that's not exported as part of 
// a different namespace.
export namespace NamespaceC
{
	using NamespaceB::ClassE;
};

// Export an alias to a type.
export using AliasD = ::NamespaceB::ClassE;
// Export an "aliased" function.
export auto constexpr AliasedFunction = ::NamespaceB::SomeFunction;
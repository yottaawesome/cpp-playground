export module somemodule;

// Export single type.
export struct ClassA {};

// Export block for multiple types.
export
{
	struct ClassB {};
	struct ClassC {};
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
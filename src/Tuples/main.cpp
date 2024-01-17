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

// https://andreasfertig.blog/2024/01/cpp20-concepts-applied/
namespace BitfieldEnums
{
	template<typename T>
	concept op_or = std::is_enum_v<T> and requires(T e)
	{
		//look for enable_bitmask_operator_or to enable this operator
		enable_bitmask_operator_or(e);
	};

	template<typename T>
	requires op_or<T>
	constexpr auto operator|(const T lhs, const T rhs)
	{
		return static_cast<T>(std::to_underlying(lhs) | std::to_underlying(rhs));
	}

	namespace Filesystem 
	{
		enum class Permission : uint8_t 
		{
			Read = 0x01,
			Write = 0x02,
			Execute = 0x04,
		};

		//Opt - in for operator|
		consteval void enable_bitmask_operator_or(Permission);
	}

	using Filesystem::Permission;
	Permission readAndWrite{ Permission::Read | Permission::Write };
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

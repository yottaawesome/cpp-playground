import tests;
import std;

namespace ReturnTypes
{
	auto&& To(auto&& v)
	{
		//return v;
		return std::forward<decltype(v)>(v);
	}

	template<typename T>
	struct M
	{
		M(auto&& f) : T(To(std::forward<decltype(f)>(f))) {}
		T t;
	};

	void Run()
	{
		std::string s;
		std::string& t = s;
		const std::string& t2 = s;
		std::println("std::string&: {}", std::same_as<decltype(To(s)), std::string&>);
		std::println("std::string&&: {}", std::same_as<decltype(To(std::string{})), std::string&& > );
		std::println("std::string&: {}", std::same_as<decltype(To(t)), std::string&>);
		std::println("const std::string&: {}", std::same_as<decltype(To(t2)), const std::string&>);
	}
}

enum class SomeEnum
{
	ValA, ValB, ValC
};

template<typename TEnum, typename S>
struct E {};

namespace B
{
	struct EnumBase : E<SomeEnum, struct F> {};
	template<auto T> struct Val {};
	struct A : EnumBase, Val<SomeEnum::ValA> {};
	struct B : EnumBase {};
	struct C : EnumBase {};

	EnumBase b = A{};
}

namespace MoreReturnTypes
{
	// We want to return either std::string& or std::string

	template<typename T>
	struct Return { Return(T) {} };

	template<>
	struct Return<std::string&>
	{
		Return(std::string& s) : S{s} {};
		using Type = std::string&;
		std::string& S;
	};

	template<>
	struct Return<std::string&&>
	{
		using Type = std::string;
		Return(std::string&& s) : S{ std::forward<std::string&&>(s) } {};
		std::string S;
	};

	template<>
	struct Return<std::string_view>
	{
		Return(std::string_view) {}
		using Type = std::string;
	};

	template<>
	struct Return<const char*>
	{
		using Type = std::string;
	};

	template<size_t N>
	struct Return<const char(&)[N]>
	{
		using Type = std::string;
	};

	template<typename T, typename...M>
	concept OneOf = (std::same_as<T, M> or ...);

	template<typename T, typename...M>
	concept ConvertibleTo = (std::convertible_to<T, M> or ...);

	template<typename T>
	concept StringType = ConvertibleTo<std::remove_cvref_t<T>, std::string, std::string_view>;
		//OneOf<std::remove_cvref_t<T>, std::string, std::string_view>;

	decltype(auto) GetString(StringType auto&& value)
	{
		using T = decltype(value);
		if constexpr (OneOf<T, std::string&, const std::string&>)
		{
			return value;
		}
		else if constexpr (std::is_constructible_v<std::string, T>)
		{
			return std::string{ std::forward<T>(value) };
		}
		else
		{
			std::println("b");
			return std::string{};
		}
	}

	template<typename T>
	using AlwaysFalse = std::false_type;


	void Run()
	{
		std::string a;


		//AnyString as(a);
		//std::println("{}", std::same_as<decltype(as.str), std::string&>);

		GetString(a);
		std::println("{}", std::same_as<decltype(GetString(a)), std::string&>);
		//std::println("{}", &GetString(a) == &a);
	}
}

namespace Search
{
	template<size_t N>
	struct FixedString
	{
		char Buffer[N];
		constexpr FixedString(const char(&arg)[N])
		{
			std::copy_n(arg, N, Buffer);
		}

		constexpr std::string_view ToView() const noexcept { return { Buffer }; }

		constexpr void Print() const noexcept { /*std::println("{}", Buffer);*/ }

		constexpr bool operator==(FixedString<N> other) const noexcept { return std::equal(other.Buffer, other.Buffer + N, Buffer); }
		template<size_t M>
		constexpr bool operator==(FixedString<M> other) const noexcept { return false; }
	};
	template<size_t N>
	FixedString(const char(&)[N]) -> FixedString<N>;

	template<FixedString...VValues>
	struct Searcher
	{
		template<FixedString F>
		constexpr auto Blah() -> void
		{
			int i = 0;
			((VValues.Print(), i++), ...);
			constexpr bool test = [...args = VValues]() constexpr -> bool
			{
				return (
					[args]() constexpr -> bool
					{
						args.Print();
						return args == F;
					}() or ...
				);
			}();
			static_assert(test, "The value was not found.");
		}
	};

	void DoIt(auto&&...stuff)
	{
		[...a = std::forward<decltype(stuff)>(stuff)]
		{
			(std::print("{}", a), ...);
		}();
	}

	void Run()
	{
		DoIt(1, 2, 3);
		Searcher<"This is a test", "And this is another test"> test;
		test.Blah<"This is a test">();
	}
}

namespace Combo 
{
	template<typename T>
	concept IntOrIntVector = std::same_as<T, int> or std::same_as<T, std::vector<int>>;

	template<typename T>
	concept Conformant = requires(const T t)
	{
		{ t() } noexcept -> IntOrIntVector;
	};

	struct AA
	{
		constexpr int operator()() const noexcept
		{
			return 1;
		}
	};
	struct BB
	{
		constexpr std::vector<int> operator()() const noexcept
		{
			return { 1 };
		}
	};
	static_assert(Conformant<AA> and Conformant<BB>, "AA and BB must conform to the Conformant concept.");

	template<typename...Ts>
	struct Overload : Ts... 
	{ 
		using Ts::operator()...; 
	};

	constexpr std::tuple<AA, BB> Tuple;

	void Proc(int) {}
	void Proc(const std::vector<int>&) {}

	constexpr auto Visit(auto&& tuple, auto&&...fn)
	{
		return std::visit(Overload{ std::forward<decltype(fn)>(fn)... }, std::forward<decltype(tuple)>(tuple));
	}

	void AnotherDo()
	{
		[&tuple = Tuple]<size_t...Is>(std::index_sequence<Is...>)
		{
			std::vector<int> collection;
			([&value = std::get<Is>(tuple), &collection]()
			{
				auto result = value();
				Proc(result);

				Visit(
					std::variant<int, std::vector<int>>{value()},
					[&collection](int i)
					{
						collection.push_back(i);
					},
					[&collection](const std::vector<int>& other)
					{
						collection.insert(std::end(collection), std::begin(other), std::end(other));
					}
				);
			}(), ...);
		}(std::make_index_sequence<std::tuple_size_v<decltype(Tuple)>>{});
	}
}

// https://stackoverflow.com/a/42774523/7448661
namespace ConcatenateArrays
{
	template <typename Type, std::size_t... sizes>
	auto concatenate(const std::array<Type, sizes>&... arrays)
	{
		std::array<Type, (sizes + ...)> result;
		std::size_t index{};

		((std::copy_n(arrays.begin(), sizes, result.begin() + index), index += sizes), ...);

		return result;
	}

	void Run()
	{
		const std::array array1 = { 1, 2, 3 };
		const std::array array2 = { 4, 5 };
		const std::array array3 = { 6, 7, 8, 9 };

		const auto result = concatenate(array1, array2, array3);
	}
}

auto main() -> int
{
	std::array s{ 1 };
	//s.be
	SizeErased::SizeErasedArray f(std::array{ 1,2,3 });

	f = std::array{ 1,2,3, 4 };
	for (int o : f)
	{
		std::println("{} ", o);
	}

	constexpr int i = 10;
	TupleAll3::Run();
	//s.Do();
	Search::Run();
	//MoreReturnTypes::Run();
	return 0;
}

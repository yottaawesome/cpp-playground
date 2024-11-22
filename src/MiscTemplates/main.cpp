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

enum class AnEnum
{
	TypeA,
	TypeB
};

template<AnEnum VEnum>
auto SomeFunc() -> auto;

template<>
auto SomeFunc<AnEnum::TypeA>() -> auto { return 1; }
template<>
auto SomeFunc<AnEnum::TypeB>() -> auto { return 1.f; }


template<typename T>
concept XX = std::same_as<T, int> or std::same_as<T, float>;

template<typename T>
auto DoIt()->XX auto
{
	return 1;
}

struct AA { void DoIt() {} };
struct AB { void DoIt() {} };
template<typename T>
concept IsDoIt = requires(T t)
{
	t.DoIt();
};

template<typename T>
using OO = T(*)();

namespace SomeRandomStuff
{
	struct I { virtual ~I() = 0 {} virtual void DoIt() = 0; };
	struct A : I { void DoIt() override {} };
	struct B : I { void DoIt() override {} };

	template<typename TCommon, typename... TArgs>
	struct Types
	{
		enum { Arity = sizeof...(TArgs) };

		constexpr static int Arity2 = sizeof...(TArgs);

		template<size_t I>
		struct ElementType
		{
			using Type = std::tuple_element_t<I, std::tuple<TArgs...>>;
		};

		template<size_t I>
		using ElementTypeT = std::tuple_element_t<I, std::tuple<TArgs...>>;

		template<size_t Index = 0>
		static auto Get(int required) -> std::unique_ptr<TCommon>
		{
			if constexpr (Index >= sizeof...(TArgs))
				throw std::runtime_error("no");
			else
				return Index == required
					? std::unique_ptr<TCommon>(new ElementTypeT<Index>)
					: Get<Index+1>(required);
		};
	};

	template<auto x>
	concept XJ = []() { return x == 1; }();

	template<auto O>
	struct K 
	{
		consteval K(int i) { O == i; }
	};

	struct LLL
	{
		LLL(XJ auto i) {  }
	};

	
	void P() {};

	struct SomethingToCapture {};
	struct SomethingElseToCapture {};

	struct Yap
	{
		Yap(auto&& t, auto&& s) : pair{ t,s } {}
		void DoA(int i) 
		{
			pair.first(i);
		}
		void DoB(float f)
		{
			pair.second(f);
		}
		std::pair<std::function<void(int)>, std::function<void(float)>> pair;
	};

	//std::pair<std::function<void(int)>, std::function<void(float)>> 
	auto Make()
	{
		SomethingToCapture s;
		return Yap{ [s](int) {}, [s](float) {} };

		SomethingElseToCapture t;
		return Yap{ [t](int) {}, [t](float) {} };

		//return { [s](int) {}, [s](float) {} };
		//return { [s](int) {}, [s](float) {} };
	}

	void Run()
	{
		auto maked = Make();
		maked.DoA(1);

		//LLL l(1);

		std::tuple<A, B> testTuple;

		using Impls = Types<I, A, B>;
		auto i = Impls::Get(1);
		i->DoIt();

		std::optional found = 
			[]<typename...T>(this auto && self, int i, T&&... values) -> auto
			{
				std::variant<T...> var;
				int curr = 0;
				([i, &var]<typename S = T>(int curr) -> bool
				{
					return i == curr ? (var = S{}, true) : false;
				}(curr++) or ...);
				return var;
			}(1, 1, 2.f);

		std::unique_ptr<I> ptr =
			[]<size_t Index = 0, typename I, typename...T>(this auto&& self, int required, Types<I, T...> tuple, std::integral_constant<int, Index> = {}) -> std::unique_ptr<I>
			{
				if constexpr (Index >= sizeof...(T))
					throw std::runtime_error("no");
				else 
					return Index == required 
						? std::unique_ptr<I>(new Types<I, T...>::ElementType<Index>::Type) 
						: self(required, tuple, std::integral_constant<int, Index + 1>{});
			}(1, Impls{});
		ptr->DoIt();

		std::unique_ptr<I> out = 
			[]<size_t Index, typename...T>(this auto && self, int i, std::tuple<T...> tuple, std::integral_constant<int, Index>) -> std::unique_ptr<I>
			{
				constexpr auto Size = sizeof...(T);
				if constexpr (Index >= Size)
					throw std::runtime_error("no");
				else return Index == i 
					? std::unique_ptr<I>(new std::tuple_element_t<Index, std::tuple<T...>>) 
					: self(i, tuple, std::integral_constant<int, Index + 1>{});
			}(1, testTuple, std::integral_constant<int, 0>{});
	}
}

namespace MultiTypes
{
	struct RType
	{
		constexpr static inline int Value = 3;
		int Value2 = 3;
	};

	template<typename T>
	concept RTypeConcept = requires(T t)
	{
		requires T::Value == 3;
	};
	static_assert(RTypeConcept<RType>);

	template<typename...TTypes>
	struct TypeSequence
	{
		std::tuple<TTypes...> Tuple;

		void DoOn(auto fn) const
		{
			([&fn, this]<typename S = TTypes>()
			{
				if (not TTypes::Active)
					return;
				if constexpr (std::invocable<decltype(fn), S>)
					fn(std::get<S>(Tuple));
			}(), ...);

			((TTypes::Active
				? [&fn, this]<typename S = TTypes>()
				{
					if constexpr (std::invocable<decltype(fn), S>)
						fn(std::get<S>(Tuple));
				}()
				: void()), ...);

			// Works, but tries to invoke it on all
			//((T::Active ? fn(std::get<T>(Tuple)) : void()), ...);
		}
	
		template<typename...TTypes>
		void PostData(std::vector<std::byte> data)
		{
			([]<typename S = TTypes>()
			{
				auto x = std::get<S>(Tuple);
			}, ...);
		}

		void PostData(std::vector<std::byte> data)
		{
			DoOn([](const auto& connection) {});
		}
	};

	struct QA { static inline bool Active = true; static constexpr int Type = 1; };
	struct AQ { static inline bool Active = true; static constexpr int Type = 2; };
	constexpr TypeSequence<QA, AQ> Sequence;

	template<typename T>
	concept OnlyType2 = requires(T t)
	{
		requires T::Type == 1;
	};

	void Run()
	{
		Sequence.DoOn([](const auto& type) {});
		Sequence.DoOn([](const AQ& type) {});
		Sequence.DoOn([](const OnlyType2 auto& type) {});
	}
}

namespace TemplatesSetup
{
	struct A {};
	struct B {};
	struct C {};
	struct D {};
	struct E {};

	template<typename A, typename B, typename C, typename D, typename E>
	int RunWithTypes()
	{
		return 0;
	}

	template<typename...TOthers>
	int SetupD()
	{
		return RunWithTypes<E, TOthers...>();
	}

	template<typename...TOthers>
	int SetupC()
	{
		return SetupD<D, TOthers...>();
	}

	template<typename...TOthers>
	int SetupB()
	{
		return SetupC<C, TOthers...>();
	}

	template<typename...TOthers>
	int SetupA()
	{
		return SetupB<B, TOthers...>();
	}

	void Run()
	{
		SetupA<A>();
	}
}

template<typename TDummy, typename T, typename...TArgs>
using F = auto(TArgs...)->T;

struct FF
{
	int X = 10;
};

constexpr FF f;

template<typename...T, auto Is = std::make_index_sequence<sizeof...(T)>{} >
void V(std::tuple<T...> t)
{

}

struct SS
{
	void operator()(int) {}
	void operator()(float) {}
};

namespace MultiVector
{

}

auto main() -> int
{
	std::any any = 1;
	int anyInt = std::any_cast<int>(any);

	std::variant<int, float> fffff;
	std::visit(SS{}, fffff);

	int x = 1;
	std::tuple t([x](auto s) { return s(x); });

	std::get<0>(t)([](auto&& s) {return s; });

	[]<int VIndex = 0>(this auto self, std::integral_constant<int, VIndex> = {})
	{
		if constexpr (VIndex < 5)
		{
			self(std::integral_constant<int, VIndex + 1>{});
		}
	}();

	std::vector<int> vint{ 1,2,3 };
	[](this auto self, int index, const std::vector<int>& vint) -> void
	{
		if (index < vint.size())
			self(++index, vint);
	}(0, vint);


	SomeRandomStuff::Run();

	AnEnum a = AnEnum::TypeA;

	IsDoIt auto aDoIt = [](int i) -> auto(*)() -> IsDoIt auto
	{
		//if (i == 0)
			return (OO<AA>) []()->IsDoIt auto {return AA{};};
		//else
			//return (OO<AB>) []()->IsDoIt auto {return AB{}; };
	}(1)();

	constexpr auto lambda = []<int N=1>() { int test[N]; };

	[](this auto&& self, int x) consteval -> void
	{
		if (x > 0)
			self(x-1);
	}(1);

	// ICE in 17.12.0
	//lambda.template operator() < 5 > ();
	//lambda.operator() < 5 > ();

	Splitter::Run();
	TypeIndexes::Run();

	using fn_t = auto()->void;
	std::tuple o{ std::function<fn_t>{ []() {} } };
	o = { [] {} };


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

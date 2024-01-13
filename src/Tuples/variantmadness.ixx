export module variantmadness;
import std;

export namespace SomeTest
{
	// See https://stackoverflow.com/questions/68839163/is-it-impossible-to-pass-a-run-time-integer-as-a-template-argument
	class Base
	{
	};

	template<int i>
	class Foo : public Base
	{
	};

	namespace SomeEnum
	{
		enum
		{
			First,
			Second,
			Third,
			Last
		};
	}

	namespace detail 
	{
		template<size_t I>
		std::unique_ptr<Base> makeForIndex() 
		{
			return std::make_unique<Foo<I>>();
		}

		template<size_t... Is>
		auto makeFoo(size_t nIdx, std::index_sequence<Is...>) 
		{
			using FuncType = std::unique_ptr<Base>(*)();
			constexpr FuncType arFuncs[] = 
			{
				detail::makeForIndex<Is>...
			};
			return arFuncs[nIdx]();
		}	
	}

	auto makeFoo(size_t nIdx)
	{
		return detail::makeFoo(nIdx, std::make_index_sequence<SomeEnum::Last>());
	}

	void Run()
	{
		size_t n = 0;
		auto ptr = makeFoo(n);
	}
}

namespace MoreTesting
{
	// See https://stackoverflow.com/questions/46450054/retrieve-value-out-of-cascading-ifs-fold-expression
	struct A { void blah() {} };
	struct B { void blah() {} };

	template<typename T>
	concept Blah = requires(T t) 
	{
		t.blah();
	};

	template <typename InputType, typename ReturnType, typename ...Pairs> 
	auto make_switch(Pairs ...ps)
	{
		/* You could do
		 *   using InputType  = std::common_type_t<typename Pairs::first_type...>;
		 *   using ReturnType = std::common_type_t<decltype(ps.second())...>;
		 * instead of using template parameters.
		 */

		return [=](InputType x)
			{
				std::optional<ReturnType> ret /* (default_value) */;
				((x == ps.first && (void(ret.emplace(std::move(ps.second()))), 1)) || ...)
					/* || (throw whatever, 1) */;
				return *ret;
			};
	}

	A DoA() { return A{}; }
	B DoB() { return B{}; }

	template<size_t N, typename Tuple>
	void SomeTest(Tuple& t)
	{

	}

	template<Blah... T>
	void Wait(size_t index = 0)
	{
		std::tuple t{
			[]() { return A{}; },
			[]() { return B{}; }
		};

		[]<typename Tuple, std::size_t... I>(Tuple && t, size_t idx, std::index_sequence<I...>)
		{
			return ((I == idx ? (std::get<I>(t)(), false) : (void(), true)) and ...);
		}(
			std::forward<decltype(t)>(t),
			index,
			std::make_index_sequence<sizeof...(T)>{}
		);
	}

	namespace detail 
	{
		template<size_t I>
		auto makeForIndex() 
		{
			return A{};
		}

		template<>
		auto makeForIndex<0>()
		{
			return A{};
		}

		template<>
		auto makeForIndex<1>()
		{
			return B{};
		}

		template<size_t... Is>
		auto makeFoo(size_t nIdx, std::index_sequence<Is...>) 
		{
			constexpr std::tuple arFuncs {
				detail::makeForIndex<Is>...
			};
			//return arFuncs();
		}
	}

	namespace E
	{
		enum 
		{
			AA, 
			BB
		};
	}

	void Run()
	{
		detail::makeFoo(1, std::index_sequence<E::BB>{});
		size_t index = 1;
		Wait<A, B>(index);
	}
}

export namespace SetVariantFromTupleDynamically
{
	struct file_writer 
	{ 
		void write() {} 
	};
	struct socket_writer
	{ 
		void write() {}
	};
	template<typename T>
	concept writeable = requires(T t)
	{
		t.write();
	};
	static_assert(writeable<file_writer> and writeable<socket_writer>);

	template<class... Ts> 
	struct overload : Ts... 
	{ 
		using Ts::operator()...; 
	};

	template<size_t Index, typename TTuple, typename TVariant>
	void instantiate(TTuple& tuple, TVariant& variant)
	{
		variant = std::get<Index>(tuple)();
	}

	void Run()
	{
		std::tuple factories{
			[] { return file_writer{}; },
			[] { return socket_writer{}; }
		};

		std::variant<file_writer, socket_writer> variants;

		size_t index = 0;

		[]<typename TTuple, typename TVariant, size_t...Is>(TTuple& tuple, TVariant& variant, size_t idx, std::index_sequence<Is...>)
		{
			(((Is == idx) ? (instantiate<Is>(tuple, variant), false) : (void(), true)) and ...);
		}(
			factories,
			variants,
			index, 
			std::make_index_sequence<std::tuple_size_v<decltype(factories)>>{}
		);

		std::visit(
			overload{ 
				[](file_writer& instance) { instance.write(); },
				[](socket_writer& instance) { instance.write(); }
			},
			variants
		);
	}
}

export namespace RuntimeSetting
{
	namespace Helper
	{
		template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
	}

	struct TagA {};
	struct TagB {};
	struct TagC {};

	template <typename TVariant, size_t N = 0>
	void RuntimeSet(TVariant& tup, size_t idx)
	{
		if (N == idx)
			tup = std::variant_alternative_t<N, TVariant>{};
		if constexpr (N + 1 < std::variant_size_v<TVariant>)
			RuntimeSet<TVariant, N + 1>(tup, idx);
	}

	void Run()
	{
		std::variant<TagA, TagB, TagC> variant;
		RuntimeSet(variant, 1);
		TagB f = std::get<1>(variant);

		std::visit(
			Helper::overload{
				[](const TagA& tag)
				{
					std::cout << "TagA...\n";
				},
				[](const TagB& tag)
				{
					std::cout << "TagB...\n";
				},
				[](const TagC& tag)
				{
					std::cout << "TagC...\n";
				}
			},
			variant
		);
	}
}
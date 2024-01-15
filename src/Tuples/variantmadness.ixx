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

export namespace IsTuple
{
	template<typename T>
	struct is_tuple : std::false_type {};

	template<typename...T>
	struct is_tuple<std::tuple<T...>> : std::true_type {};

	template<typename...T>
	constexpr bool is_tuple_v = is_tuple<T...>::value;

	template<typename T>
	concept tuple_like = is_tuple_v<T>;

	std::tuple t{ 1, 2 };
	static_assert(tuple_like<decltype(t)>);

	template<typename T>
	struct is_variant : std::false_type {};

	template<typename...T>
	struct is_variant<std::variant<T...>> : std::true_type {};

	template<typename...T>
	constexpr bool is_variant_v = is_variant<T...>::value;

	template<typename T>
	concept variant_like = is_variant_v<T>;

	std::variant<int, std::string> v{};
	static_assert(variant_like<decltype(v)>);
}

export namespace MoreTesting
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

	void Wait()
	{
		std::tuple events{
			[]() { return A{}; },
			[]() { return B{}; }
		};

		size_t index = 0;

		[]<typename Tuple, size_t...I>(Tuple&& t, size_t idx, std::index_sequence<I...>)
		{
			return ((I == idx ? (std::get<I>(t)(), false) : (void(), true)) and ...);
		}(
			std::forward<decltype(events)>(events),
			index,
			std::make_index_sequence<std::tuple_size_v<decltype(events)>>{}
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
		Wait();
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
	auto instantiate(TTuple& tuple, TVariant& variant)
	{
		variant = std::get<Index>(tuple)();
		//return std::get<Index>(tuple)();
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
			return (((Is == idx) ? (instantiate<Is>(tuple, variant), false) : (void(), true)) and ...);
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

namespace ReadLineLoop
{
	template<typename T>
	struct IsTupleLike : std::false_type {};

	template<typename...T>
	struct IsTupleLike<std::tuple<T...>> : std::true_type {};

	template<typename T, size_t I>
	struct IsTupleLike<std::array<T, I>> : std::true_type {};

	template<typename T>
	constexpr bool IsTupleLikeV = IsTupleLike<T>::value;

	template<typename T>
	concept TupleLike = IsTupleLike<T>::value;

	std::tuple events
	{
		[]() { std::println("You entered 0..."); },
		[]() { std::println("You entered 1..."); },
		[]() { std::println("You entered 2..."); }
	};

	bool LoopRun()
	{
		using events_t = decltype(events);
		std::println("Please enter a number between 0 and less than {} or e to quit.", std::tuple_size_v<events_t>);
		std::string line;
		std::getline(std::cin, line);
		if (line == "e")
			return false;

		const int value = std::stoi(line);
		if (value < 0)
		{
			std::println("Invalid negative index {}", value);
			return true;
		}

		const bool found = []<size_t...I>(TupleLike auto&& tuple, size_t index, std::index_sequence<I...>, auto&&...args)
		{
			return (
				(I == index 
					? (std::get<I>(tuple)(std::forward<decltype(args)>(args)...), true) 
					: (void(), false)
				) or ...
			);
		}(
			std::forward<events_t>(events),
			value,
			std::make_index_sequence<std::tuple_size_v<events_t>>{}
		);
		if (not found)
			std::println("Didn't find an entry for index {}...", value);

		return true;
	}

	auto make(auto& callable, auto&&...args)
	{
		return [callable, ...args = std::forward<decltype(args)>(args)] { return callable(args...); };
	}

	auto make_safe(auto& callable, auto&&...args)
	{
		return [callable, ...args = std::forward<decltype(args)>(args)]
		{
			try
			{
				callable(std::forward<decltype(args)>(args)...);
				return true;
			}
			catch (...)
			{
				return false;
			}
		};
	}

	auto make_retry_safe(auto& callable, unsigned retries, auto&&...args)
	{
		return [callable, retries, ...args = std::forward<decltype(args)>(args)] 
		{
			for (int i = 1; i <= retries; ++i)
			{
				try
				{
					callable(std::forward<decltype(args)>(args)...);
					return true;
				}
				catch (...)
				{
					std::this_thread::sleep_for(std::chrono::seconds{ i * 2 });
				}
			}
			return false;
		};
	}

	export void Run()
	{
		while (true) try
		{
			if (not LoopRun())
				break;
		}
		catch (const std::exception& ex)
		{
			std::println("Error: {}", ex.what());
		}

		std::println("Goodbye...");
	}
}

export namespace InvertedFind
{
	// Based on https://devblogs.microsoft.com/oldnewthing/20200629-00/?p=103910
	template<typename T, typename Tuple>
	struct tuple_element_index_helper;

	template<typename T>
	struct tuple_element_index_helper<T, std::tuple<>>
	{
		static constexpr std::size_t value = 0;
	};

	template<typename T, typename... Rest>
	struct tuple_element_index_helper<T, std::tuple<T, Rest...>>
	{
		static constexpr std::size_t value = 0;
		using RestTuple = std::tuple<Rest...>;
		static_assert(
			tuple_element_index_helper<T, RestTuple>::value ==
			std::tuple_size_v<RestTuple>,
			"type appears more than once in tuple");
	};

	template<typename T, typename First, typename... Rest>
	struct tuple_element_index_helper<T, std::tuple<First, Rest...>>
	{
		using RestTuple = std::tuple<Rest...>;
		static constexpr std::size_t value = 1 +
			tuple_element_index_helper<T, RestTuple>::value;
	};

	template<typename T, typename Tuple>
	struct tuple_element_index
	{
		static constexpr std::size_t value =
			tuple_element_index_helper<T, Tuple>::value;
		static_assert(value < std::tuple_size_v<Tuple>,
			"type does not appear in tuple");
	};

	template<typename T, typename Tuple>
	inline constexpr std::size_t tuple_element_index_v
		= tuple_element_index<T, Tuple>::value;

	void Run()
	{
		// index = 1
		constexpr std::size_t index = tuple_element_index_v<int, std::tuple<char, int, float>>;

		// error: type does not appear in tuple
		//constexpr std::size_t index = tuple_element_index_v<double, std::tuple<char, int, float>>;

		// error: type appears more than once in tuple
		//constexpr std::size_t index = tuple_element_index_v<int, std::tuple<char, int, int>>;
	}
}
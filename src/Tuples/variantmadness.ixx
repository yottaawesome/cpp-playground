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

	template<auto...Ts>
	struct H
	{
		std::tuple<decltype(Ts)...> Tuple;
		std::variant<std::invoke_result_t<decltype(Ts)>...> Variant;
	};

	auto j() { return file_writer{}; };
	auto i() { return socket_writer{}; };

	template<typename T>
	concept GG = requires(T t) { t.dos(); };

	struct D { void dos() {} };
	struct F { void dos() {} };

	constinit GG auto m = []() -> GG auto { return D{}; }();


	struct B { virtual ~B() = default; constexpr B() = default; virtual constexpr int Get() const { return 0; } };
	struct C : public B { constexpr C() = default; virtual constexpr int Get() const override { return 1; } };



	/*template<size_t I, size_t M>
	auto K(size_t index, auto& tuple)
	{
		if (index == M)
			return std::get<I>(tuple);
		else
			if constexpr (I +1 < M)
				return K<I + 1, M>(index, tuple);
		throw std::runtime_error("");
	}*/

	void Run()
	{
		std::tuple factories{
			[] { return file_writer{}; },
			[] { return socket_writer{}; }
		};

		const B& b = C();
		//std::get<b->Get()>(factories);

		//K<0, 2>(1, factories);

		H<j, i> ll{ .Tuple{j,i} };

		std::variant<file_writer, socket_writer> variants;

		size_t index = 0;

		[]<typename TTuple, typename TVariant, size_t...Is>(TTuple& tuple, TVariant& variant, size_t idx, std::index_sequence<Is...>)
		{
			([](TTuple& tuple, TVariant& variant, size_t idx)
			{
				if (idx == Is)
				{
					variant = std::get<Is>(tuple)();
					return true;
				}
				return false;
			}(tuple, variant, idx) or ...);

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

export namespace SomeOtherTest
{
	struct A {};
	struct B {};
	struct C {};

	template<typename T>
	struct b_type
	{
		T Value = {};
		bool Found = false;
		operator bool() { return Found; }
	};

	void Run()
	{
		std::tuple t{ A{}, B{}, C{} };

		std::variant<A, B, C> v{};
		bool b = holds_alternative<A>(v);
		v = B{};

		auto index = v.index();

		using m = std::variant_alternative_t<0, decltype(v)>;

		auto item = []<size_t...Is>(auto& variant, size_t index, std::index_sequence<Is...>)
		{
			return (
				[]<size_t I = Is>(size_t index, auto&& variant)
				{
					using type = std::variant_alternative_t<I, std::remove_cvref_t<decltype(variant)>>;
					if (I == index)
					{
						return std::get<I>(variant);
					}
					return type{};
				}(index, std::forward<decltype(variant)>(variant)), ...
			);
		}(
			v,
			index,
			std::make_index_sequence<std::variant_size_v<decltype(v)>>{}
		);

		//std::println("{}", typeid(item).name());
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

export namespace MakeSafe
{
	auto make(auto&& callable, auto&&...args)
	{
		return [callable, ...args = std::forward<decltype(args)>(args)] { return std::invoke(callable, args...); };
	}

	auto make_safe(auto&& callable, auto&&...args)
	{
		return [callable, ...args = std::forward<decltype(args)>(args)]
		{
			try
			{
				std::invoke(callable, args...);
				return true;
			}
			catch (...)
			{
				return false;
			}
		};
	}

	auto make_retry_safe(auto&& callable, unsigned retries, auto&&...args)
	{
		return [callable, retries, ...args = std::forward<decltype(args)>(args)]
		{
			for (int i = 1; i <= retries; ++i)
			{
				try
				{
					std::invoke(callable, args...);
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

	struct A { void blah() const {} };

	void SomeFunc(const A& a) { a.blah(); }
	void SomeFunc2(int a) {  }
	void SomeFunc3() {  }

	auto F1() -> auto
	{
		// ICE
		//return make_safe(SomeFunc, A{});
	}

	void Run()
	{
		A a{};
		make_safe(SomeFunc, std::ref(a))();
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

export namespace RandomStuff
{
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

	struct db { void save() {} void get() {} };
	struct file { void save() {} void get() {} };

	template<typename T>
	concept sinkable = requires(T t)
	{
		t.get();
		t.save();
	};

	struct saver
	{
		saver(int x)
		{
			if (x == 1)
				sinks = db{};
			else
				sinks = file{};
		}

		auto save()
		{
			return std::visit(
				[](sinkable auto&& t) { return t.save(); },
				sinks
			);
			//sinks.visit([](sinkable auto&& t) { t.save(); });
		}

		void get()
		{
			std::visit(
				[](sinkable auto&& t) { return t.get(); },
				sinks
			);
		}

		std::variant<db, file> sinks;
	};

	struct A {};
	struct B {};

	void Get()
	{
		int x = 1;
		std::vector v{ 0,1,2,3,4 };
		auto f = []<size_t...I>(size_t index, std::index_sequence<I...>) -> decltype(auto)
		{
			//return ((index == I ? [] {} : [] {}) and ...);
			//return ((index == I ? [] {return A{}; }() : [] { return B{}; }()), ...);
			//return ((index == I ? A{} : (B{})) and ...);

			return ([]<size_t T = I>()
			{
				if constexpr (T == 1)
				{
					return A{};
				}
				else
				{
					return B{};
				}
			}(), ...);
		}(x, std::make_index_sequence<2>{});

		std::cout << typeid(f).name() << std::endl;
	}
}

export namespace OtherStuff
{
	// https://stackoverflow.com/questions/77070107/templated-way-to-match-stdvariant-index-to-type-at-runtime
	template<std::size_t I>
	using index_t = std::integral_constant<std::size_t, I>;

	template<std::size_t I>
	constexpr index_t<I> index_v{};

	template<std::size_t...Is>
	using indexes_t = std::variant<index_t<Is>...>;

	template<std::size_t...Is>
	constexpr indexes_t<Is...> get_index(std::index_sequence<Is...>, std::size_t I) {
		constexpr indexes_t<Is...> retvals[] = {
		  index_v<Is>...
		};
		return retvals[I];
	}

	template<std::size_t N>
	constexpr auto get_index(std::size_t I) {
		return get_index(std::make_index_sequence<N>{}, I);
	}

	template<size_t I, size_t M>
	auto Q(auto&& tuple)
	{
		using t = std::remove_cvref_t<decltype(tuple)>;
		std::variant v = std::apply([](auto...T) {return std::variant<decltype(T)...>{}; }, tuple);

		//std::variant v{ std::forward_as_tuple<t>(tuple) };
		if constexpr (I == M)
			return tuple;
		else
			return Q<I + 1, M>(std::tuple_cat(tuple, std::tuple{ std::make_index_sequence<I>{} }));
	}

	void Run()
	{
		std::tuple<int> ttt;
		auto vvv = Q<1, 5>(std::forward<decltype(ttt)>(ttt));
		std::cout << typeid(vvv).name() << "\n";
		std::variant<int, float, double > v;
		using M = decltype(v);
		auto r = std::visit(
			[&](auto I)
			{
				auto T = std::variant_alternative_t<I, M>{};
				//return std::get<T>(v);
				return sizeof(char[I + 1]); // +1 is needed, as arrays of size 0 are illegal
			},
			get_index<3>(1)
		);
		std::cout << r << "\n";

		auto i = get_index<3>(1);
		std::cout << typeid(i).name() << "\n";
		/*[](auto I)
		{
			auto T = std::variant_alternative_t<I, M>{};
		}(get_index<3>(1));*/
	}
}

export namespace EvenMoreFunnyStuff
{
	template<class... Ts>
	struct overload : Ts... { using Ts::operator()...; };

	struct A
	{
		virtual ~A() = default;
		virtual int Blah() = 0;
	};
	struct B final : public A { int Blah() override { return std::rand() % 50; } };
	struct C final : public A { int Blah() override { return std::rand() % 50; } };
	struct D final : public A { int Blah() override { return std::rand() % 50; } };
	struct E final { int Blah() { return std::rand() % 50; } };

	template<typename T>
	concept IsInterface = requires(T t)
	{
		t.Blah();
	};

	void Run()
	{
		std::variant<B, C, D> v = B{};

		constexpr int i = 1;

		if constexpr (i == 1)
		{
			IsInterface auto a = E{};
			auto n = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < 5000000; i++)
				int x = a.Blah();
			std::println("{}", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - n));
		}
		else if constexpr (i == 2)
		{
			A* a2 = new B();
			auto n = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < 5000000; i++)
				int x = a2->Blah();
			std::println("{}", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - n));
		}
		else if constexpr (i == 3)
		{
			IsInterface auto& a = std::visit(
				overload{
					[](B& a) -> A& { return a; },
					[](C& a) -> A& { return a; },
					[](D& a) -> A& { return a; }
				},
				v
			);

			auto n = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < 5000000; i++)
				int x = a.Blah();
			std::println("{}", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - n));
		}
	}
}

export namespace MoreStuff
{
	template <typename ... Ts, std::size_t ... Is>
	std::variant<Ts...> get_impl(std::size_t index, std::index_sequence<Is...>, const std::tuple<Ts...>& t)
	{
		using getter_type = std::variant<Ts...>(*)(const std::tuple<Ts...>&);
		getter_type funcs[] = {
			+[](const std::tuple<Ts...>& tuple) -> std::variant<Ts...>
			{
				return std::get<Is>(tuple);
			}...
		};

		return funcs[index](t);
	}

	template <typename ... Ts>
	std::variant<Ts...> get(std::size_t index, const std::tuple<Ts...>& t)
	{
		return get_impl(index, std::index_sequence_for<Ts...>(), t);
	}

	template<typename...TArgs>
	struct overloaded : public TArgs...
	{
		using TArgs::operator()...;
	};

	struct A
	{
		A() = default;
		A(int, std::string) {}
	};
	struct B
	{
		B() = default;
		B(int) {}
	};
	struct C {};

	void Run()
	{
		int argA1 = 1;
		std::string argA2 = "a";
		int argB1 = 2;
		// ...

		using type_tuple = std::tuple<A, B, C>;
		type_tuple t{};
		int i = 0;

		auto obj = std::visit(
			overloaded{
				[&](const A&) -> std::variant<A, B, C> { return A(argA1, argA2); },
				[&](const B&) -> std::variant<A, B, C> { return B(argB1); },
				[&](const C&) -> std::variant<A, B, C> { return C(); },
			},
			get(i, t)
			);
	}
}
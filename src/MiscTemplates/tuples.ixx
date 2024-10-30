export module tuples;
import std;

export namespace TupleAll
{
	template<typename T, typename...Ts>
	concept IsSame = (std::same_as<T, Ts> and ...);

	constexpr auto All =
		[](auto&&... args) constexpr
		{
			if constexpr (sizeof...(args) == 0)
				return true;
			else
				return IsSame<decltype(args)...>;
		};

	void Run()
	{
		constexpr bool b = std::apply(All, std::tuple{ 1, 2 });
		constexpr bool c = std::apply(All, std::tuple{ 1, 2.f });
		std::println("{} {}", b, c);
	}
}

export namespace TupleAll2
{
	constexpr auto All =
		[](auto&& a, auto&&... args) constexpr
		{
			return (std::same_as<decltype(a), decltype(args)> and ...);
		};

	constexpr auto All2 =
		[](auto&&... args) constexpr
		{
			if constexpr (sizeof...(args) == 0 or sizeof...(args) == 1)
				return true;
			else
				return All(args...);
		};


	void Run()
	{
		constexpr bool b = std::apply(All, std::tuple{ 1, 2 });
		constexpr bool c = std::apply(All, std::tuple{ 1, 2.f });
		std::println("{} {}", b, c);
	}
}

export namespace TupleAll3
{
	constexpr bool Something(const std::tuple<>&) { return true; };

	template<typename T, typename...TRest>
	constexpr bool Something(const std::tuple<T, TRest...>&) { return (std::same_as<T, TRest> and ...); };

	void Run()
	{
		std::println(
			"{} {} {}",
			Something(std::tuple{ }),
			Something(std::tuple{ 1, 2, 3 }),
			Something(std::tuple{ 1, 2, 3.f })
		);
	}
}

export namespace TupleAll4
{
	template<typename...T>
	constexpr bool AllSame(const std::tuple<T...>& tuple)
	{
		if constexpr (sizeof...(T) == 0)
			return true;
		else
			return[]<typename THead, typename...TRest>(const std::tuple<THead, TRest...>&) constexpr
			{
				return (std::same_as<THead, TRest> and ...);
			}(tuple);
	}

	void Run()
	{
		AllSame(std::tuple{ });
		AllSame(std::tuple{ 1 });
	}
}

export namespace TypeIndexes
{
	template<typename... TArgs>
	struct Types
	{
		enum { Arity = sizeof...(TArgs) };

		constexpr static int Arity2 = sizeof...(TArgs);

		template<size_t I>
		struct ElementType
		{
			using Type = std::tuple_element_t<I, std::tuple<TArgs...>>;
		};

		static void Invoke(auto&& f)
		{
			//f.operator()<TArgs...>();
		}

		static auto At(int x, auto&& func)
		{
			int iter = 0;
			return ([x, &iter, &func]<typename T = TArgs>()
			{
				if (iter >= Arity)
					throw std::runtime_error("Not found");
				if (x == iter)
				{
					func(T{});
					return true;
				}
				iter++;
				return false;
			}() or ...);

			return ([]<size_t...Is>(int x, std::index_sequence<Is...>)
			{
				return ([]<typename T = TArgs>(int x)
				{
					if (x == Is)
					{
						std::println("Found it!");
						return true;
					}
					return false;
				}(x) or ...);
			}(x, std::make_index_sequence<Arity>{}));
		}

		static void Each()
		{
			([]<typename T = TArgs>()
			{
				if constexpr (std::constructible_from<T, int>)
					std::println("Type is constructible with int.");
				else
					std::println("Type is default constructible.");
			}(), ...);
		}

		void ForEach()
		{
			Types<TArgs...>::Each();
		}
	};

	struct A
	{
		//A(int) {}
	};

	struct B
	{
		void operator ()() {}
	};

	struct C
	{
		void operator ()() {}
	};

	template<typename...T>
	void AnotherOne() {}

	void Run()
	{
		using F = Types<A, B, C>;
		F::At(1, [](auto&& a) {std::println("OK!"); });

		F::ElementType<0>::Type o;
		F f{};
		f.Each();
		f.Invoke([]<typename...T>() { std::println("something something"); });
		f.Invoke(AnotherOne<int>);

		std::make_index_sequence<F::Arity> a{};
	}
}

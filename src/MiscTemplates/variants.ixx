export module variants;
import std;

export namespace VariantsA
{
	struct A
	{
		void operator()(float) {}
	};

	struct B
	{
		void operator()(double) {}
	};

	struct S : A, B
	{
		std::variant<int, float, double> F;

		void operator()(int) {}

		void Do()
		{
			std::visit(*this, F);
		}
	};
}

export namespace VariantsB
{
	template<typename...Ts>
	struct Overload : Ts...
	{
		using Ts::operator()...;
	};

	void Run()
	{
		std::variant<int, float> v;

		std::visit(
			Overload{
				[](int) {},
				[](float) {},
			}, 
			v
		);
	}
}

export namespace VariantsC
{
	void Run()
	{
		std::variant<int, float> v;
		std::visit([](auto) {}, v);
	}
}

export namespace VariantsD
{
	struct A
	{
		void operator()(int) {}
	};

	struct B
	{
		void operator()(float) {}
	};

	template<typename...Ts>
	struct Overload : Ts...
	{
		using Ts::operator()...;
	};

	using Handler = Overload<A, B>;

	void Run()
	{
		std::variant<int, float> v;
		std::visit(Handler{}, v);
	}
}

export namespace VariantsE
{
	struct State
	{
		static inline std::variant<int, float> Variant{};
	};

	struct A : State
	{
		void operator()(int) {}
	};

	struct B : State
	{
		void operator()(float) {}
	};

	template<typename...Ts>
	struct Overload : Ts...
	{
		using Ts::operator()...;
	};

	using Handler = Overload<A, B>;

	void Run()
	{
		std::visit(Handler{}, State::Variant);
	}
}

export namespace VariantOfVariants
{
	struct SocketMessageA {};
	struct SocketMessageB {};
	struct HttpMessageA {};
	struct HttpMessageB {};

	using SocketMessage = std::variant<SocketMessageA, SocketMessageB>;
	using HttpMessage = std::variant<HttpMessageA, HttpMessageB>;
	using AnyMessage = std::variant<SocketMessage, HttpMessage>;

	template<typename T>
	concept Convertible = std::convertible_to<T, SocketMessage> or std::convertible_to<T, HttpMessage>;

	template<typename...T>
	struct Overload : T...
	{
		using T::operator()...;
	};

	struct Messages
	{
		Messages() = default;

		Messages(Convertible auto&& a) : Any(a) {};

		Messages& operator=(Convertible auto&& a)
		{
			Any = a;
			return *this;
		}

		auto ExpectOne(auto&& fn)
		{
			return std::visit(
				Overload{
					[fn = std::forward<decltype(fn)>(fn)](auto& s)
					{
						std::visit(Overload{ fn, [](auto&&) { throw std::runtime_error("Wrong message type."); } }, s);
					}
				},
				Any
			);
		}

		auto ExpectHttp(auto&&...fn)
		{
			return std::visit(
				Overload{
					[](SocketMessage& s)
					{
						throw std::runtime_error("Wrong message type");
					},
					[...fn = std::forward<decltype(fn)>(fn)](HttpMessage& s)
					{
						std::visit(Overload{ fn... }, s);
					}
				},
				Any
			);
		}

		auto ExpectSocket(auto&&...fn)
		{
			return std::visit(
				Overload{
					[...fn = std::forward<decltype(fn)>(fn)](SocketMessage& s)
					{
						return std::visit(Overload{ fn... }, s);
					},
					[](HttpMessage&)
					{
						throw std::runtime_error("Wrong message type");
					}
				},
				Any
			);
		}

		auto ExpectAny(auto&&...fn)
		{
			return std::visit(
				Overload{
					[...fn = std::forward<decltype(fn)>(fn)](auto& s)
					{
						return std::visit(Overload{ fn... }, s);
					}
				},
				Any
			);
		}

		AnyMessage Any;
	};

	struct A { void Save(this auto&&, int) {} };
	struct B { void Save(this auto&&, int) {} };

	auto Get(auto X)
	{
		if constexpr (X == 1)
		{
			return A{};
		}
		else
		{
			return A{};
		}
	}

	template<int VValue>
	using Constant = std::integral_constant<int, VValue>;


	struct Database
	{
		constexpr Database(std::convertible_to<decltype(Database::Type)> auto&& arg)
			: Type(arg)
		{ }

		constexpr void Save(this auto& self, int i)
		{
			self.Visit([i](auto&& a) { a.Save(i); } );
		}

		constexpr auto& operator=(std::convertible_to<Database::Type> auto&& a)
		{
			Type = a;
			return *this;
		}

		constexpr auto Visit(this auto& self, auto&&...fn)
		{
			return std::visit(Overload{ std::forward<decltype(fn)>(fn)...}, self.Type);
		}

		std::variant<A, B> Type;
	};


	template<typename TDatabase>
	concept IsDatabase = requires(TDatabase t)
	{
		t.Save(1);
	};
	
	template<typename T>
	concept HasConstexprVoidBar = requires(T t) 
	{
		{ std::bool_constant<(T{} == std::integral_constant<int, 0>{}, true)> () } -> std::same_as<std::true_type>;
	};

	template<typename M, auto VValue>
	concept Is = []() { return M{} == VValue; }();

	constexpr auto GetDb(auto i) -> IsDatabase auto
	{
		if constexpr (Is<decltype(i), 0>)
		{
			return A{};
		}
		else if constexpr (Is<decltype(i), 1>)
		{
			return B{};
		}
		else
		{
			if (i == 0)
				return Database{ A{} };
			else
				return Database{ B{} };
		}
	}

	auto Arrays(int m)
	{
		using IntegralVariant = std::variant<Constant<0>, Constant<1>>;
		constexpr std::variant<Constant<0>, Constant<1>> constants = Constant<0>{};
		constexpr int limit = std::variant_size_v<decltype(constants)>;

		constexpr std::array Array{ 
			IntegralVariant{Constant<0>{}},
			IntegralVariant{Constant<1>{}}
		};

		constexpr int i = 0;
		constexpr Constant<Array[i].index()> aaaaa;

		std::get<Array[i].index()>(constants);
		constexpr int j = 0;
		auto integral = std::get<Array[j].index()>(constants);

		[](auto integral)
		{
			if constexpr (integral == 1)
			{
				return A{};
			}
			else
			{
				return B{};
			}
		}(integral);
	}

	struct C { virtual void Do() = 0; };
	struct D : C { void Do() override {} };
	struct E : C {};

	consteval auto M()
	{
		B a = GetDb(std::integral_constant<int, 1>{});
	}

	void Run()
	{
		M();
		auto x = GetDb(0);

		IsDatabase auto j = Database{ A{} };
		j.Save(1);

		Messages a = SocketMessageA{};
		a.ExpectOne(
			[](HttpMessageA)
			{

			}
		);

		a.ExpectHttp(
			[](HttpMessageA)
			{

			},
			[](HttpMessageB)
			{

			}
		);

		a.ExpectAny(
			[](HttpMessageA)
			{
				return 0;
			},
			[](HttpMessageB)
			{
				return 0;
			},
			[](SocketMessageA)
			{
				return 0;
			},
			[](SocketMessageB)
			{
				return 0;
			}
		);
	}

	struct Demo
	{
		virtual ~Demo() = default;
		virtual void Do() = 0;
	};

	struct Impl : Demo
	{
		void Do() override {}
	};
}

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

	void Run()
	{
		Messages a = SocketMessageA{};
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

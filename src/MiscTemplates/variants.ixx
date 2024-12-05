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

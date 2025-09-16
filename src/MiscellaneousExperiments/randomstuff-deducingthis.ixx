export module randomstuff:deducingthis;
import std;

export namespace DeducingThis
{
	struct F
	{
		// https://stackoverflow.com/questions/69335493/what-is-the-difference-between-returning-auto-and-decltypeauto
		decltype(auto) Do(this auto&& self)
		{
			return std::forward_like<decltype(self)>(self.Prop);
		}

		void Stuff(this auto* self)
		{

		}

		void M(this const F&) {}

		std::vector<int> Prop;

	private:
		int stuff = 0;
	};


	void Fn(const F& f)
	{
		F m;
		m.Do();
		f.Do();
		std::invocable<F> auto l = &F::M;
		std::invoke(&F::M, f);
	}
}

export namespace Crtp
{
	template<typename T>
	concept Printable = requires(T t) 
	{ 
		{ t.Get() } -> std::convertible_to<int>; 
	};

	struct Crtp
	{
		void Print(this Printable auto&& self)
		{
			std::println("This type is {}", self.Get());
		}

		void AndAlso(this auto&& self)
		{
			self.Impl();
		}
	};

	struct APrintable : Crtp
	{
		int Get() { return 1; }
	};

	void Run()
	{
		APrintable a;
		a.Print();
	}
}

// See also moreformatting.ixx
#pragma region Formatting
export namespace Printing
{
	template<typename T>
	concept Formattable = requires(T t)
	{
		{ t.ToString() } -> std::convertible_to<std::string>;
	};

	struct PrintTest
	{
		std::string ToString(this const auto& self)
		{ 
			return "Hello, world!"; 
		}
	};
	static_assert(Formattable<PrintTest>);
}

export namespace std
{
	template<Printing::Formattable TPrintable>
	struct formatter<TPrintable, char> : formatter<char, char>
	{
		template <class TContext>
		auto format(const TPrintable& n, TContext&& ctx) const
		{
			return format_to(ctx.out(), "{}", n.ToString());
		}
	};
}

export namespace Printing
{
	void Run()
	{
		PrintTest pt;
		std::println("{}", pt);
	}
}
#pragma endregion Formatting
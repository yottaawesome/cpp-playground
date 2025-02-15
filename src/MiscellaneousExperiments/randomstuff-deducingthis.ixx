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
		std::invoke(F::M, f);
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
	};

	struct APrintable : Crtp
	{
		int Get() { return 1; }
	};
}

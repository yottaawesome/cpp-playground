export module coroutines;
export import :threadswitch;
export import :threadswitch2;
export import :generator;
export import :eagerfuture;
export import :minimalasyncframework;
export import :taskscheduler;
export import :random;
export import :tumble;
import std;

export namespace BBB
{
	struct F
	{
		auto&& Do(this auto&& self)
		{
			return std::forward_like<decltype(self)>(self.Prop);
		}

		void Lol(this auto* self)
		{

		}

		void M(this const F&) {}

		std::vector<int> Prop;
	};


	void Fn(const F& f)
	{
		F m;
		m.Do();
		f.Do();
		//auto l = &F::M;
		std::invoke(F::M, f);
	}
}
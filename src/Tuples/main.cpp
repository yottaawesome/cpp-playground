import std;
import std.compat;
import tupletoparameterpack;
import variantmadness;

namespace RandomStuff
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

		void save()
		{
			std::visit(
				[](sinkable auto&& t)
				{
					t.save();
				},
				sinks
			);
			//sinks.visit([](sinkable auto&& t) { t.save(); });
		}

		void get()
		{
			std::visit(
				[](sinkable auto&& t)
				{
					t.get();
				},
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

namespace MoreStuff
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

int main()
{
	SomeOtherTest::Run();
	MoreStuff::Run();
	MakeSafe::Run();
	//LoopOverTypes::Run();
	//Chained::Run();
	LoopOverLambdas::Run();
	//GetTupleElement::Run();
	//TupleFunc::Run();
	//SomeTest::Run();
	ReadLineLoop::Run();
	
	return 0;
}

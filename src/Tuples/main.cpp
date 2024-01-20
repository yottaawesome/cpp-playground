import std;
import std.compat;
import tupletoparameterpack;
import variantmadness;

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

//template<size_t T>
//std::index_sequence<T> Get2(size_t t)
//{
//		return t == T ? std::make_index_sequence<T>{} : Get2<T + 1>(t);
//}

void Get()
{
	//Get2<2>(1);
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

int main()
{
	Get();
	saver s(1);
	s.save();
	s.get();

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

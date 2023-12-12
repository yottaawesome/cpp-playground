import std;
import std.compat;
import randomstuff;

template<typename T>
concept IsString =
	std::is_same_v<std::remove_reference_t<T>, std::string>
	or std::is_same_v<std::remove_reference_t<T>, std::string_view>;

struct MessageLocationTrace
{
	std::string_view Message;
};

int Something(auto&& t)
{
	if constexpr (IsString<decltype(t)>)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

template<int v, int t>
constexpr auto F(auto& tup)
{
	if constexpr (v == t - 1)
	{
		return std::get<v>(tup);
	}
	else
	{
		return F<v + 1, t>(tup);
	}
}

template <class Tuple, size_t N = 0>
void Get(auto& var, Tuple& tup, size_t idx)
{
	if (N == idx)
	{
		var = std::get<N>(tup);
	}
	if constexpr (N + 1 < std::tuple_size_v<Tuple>)
	{
		Get<Tuple, N + 1>(var, tup, idx);
	}
}

template <class Tuple, size_t N = 0>
auto Get()
{
	/*Tuple t;
	if constexpr (N < std::tuple_size<Tuple>)
	{
		return std::get<N>(t);
	}
	else
	{
		return 1;
	}*/
	//return std::tuple_element(N, Tuple);
}

template<typename...Args>
void Blah(std::variant<Args...>& m, int x, Args&&...args)
{
	std::tuple t{ args... };
	Get<decltype(t)>(m, t, x);
}

int main()
{
	std::variant<int, char> v;
	Blah(v, 0, 1, 'c');

	MessageLocationTrace m;
	Something(m.Message);
	
	return 0;
}

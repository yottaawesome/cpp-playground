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

void WorkingWithStringViews()
{
	std::string test{ "abbakadabra" };
	std::string to_find{ "a" };
	std::vector<std::string_view> results =
		[](const std::string& test, const std::string& to_find)
		{
			std::vector<std::string_view> results;
			if constexpr (true) // version A, for-loop
			{
				for (size_t pos = test.find(to_find); pos != std::string::npos; pos = test.find(to_find, pos))
				{
					results.push_back(std::string_view{ test.data() + pos, to_find.size() });
					pos += to_find.size();
				}
			}
			else // version B, while-loop
			{
				size_t pos = test.find(to_find);
				while (pos != std::string::npos)
				{
					results.push_back(std::string_view{ test.data() + pos, to_find.size() });
					pos += to_find.size();
					pos = test.find(to_find, pos);
				}
			}
			return results;
		}(test, to_find);

	std::println("Results {}", results.size());
	std::for_each(
		results.begin(), results.end(),
		[](std::string_view v)
		{
			std::println("{}", v);
		}
	);

	std::string test2 = "some=value,another=one,and=onemore,hey=again";
	results = [](const std::string& str)
		{
			std::vector<std::string_view> results;
			auto tokens = std::array{ "some=","another=","and=","hey=" };
			for (size_t pos = 0; std::string_view sv : tokens)
			{
				pos = str.find(sv, pos);
				if (pos == std::string::npos)
					continue;
				size_t comma = str.find(",", pos);
				if (comma == std::string::npos)
					results.push_back({ str.data() + pos + sv.size(), (size_t)std::distance(str.begin() + pos + sv.size(), str.end()) });
				else
					results.push_back({ str.data() + pos + sv.size(), comma - pos - sv.size() });
			}
			return results;
		}(test2);

		std::println("Found {}", results.size());
		std::for_each(
			results.begin(), results.end(),
			[](std::string_view v)
			{
				std::println("{}", v);
			}
		);
}

int main()
{
	std::variant<int, char> v;
	Blah(v, 0, 1, 'c');

	MessageLocationTrace m;
	Something(m.Message);
	DeducingThis::Run();
	Dates::Run();

	return 0;
}

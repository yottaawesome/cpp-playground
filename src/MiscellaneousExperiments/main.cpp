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

namespace pipes
{
	// Adapted from https://www.cppstories.com/2024/pipe-operator/
	// with compilation fixes.
	struct s
	{
		int x = 0;
		int y = 0;
	};

	template <typename T, typename Function> requires (std::invocable<Function, T>)
	constexpr auto operator|(T&& t, Function&& f) -> typename std::invoke_result_t<Function, T>
	{
		return std::invoke(std::forward<Function>(f), std::forward<T>(t));
	}

	auto do_something_with_s(auto&& s_to_use)
	{
		s_to_use.x = 10;
		s_to_use.y = 10;
		return s_to_use;
	}

	auto print_s(auto&& s_to_use)
	{
		std::println("{} {}", s_to_use.x, s_to_use.y);
		return s_to_use;
	}

	void run()
	{
		s ss;
		//(ss | do_something_with_s | print_s);
	}
}

namespace pipe_expected
{
	// Adapted from https://www.cppstories.com/2024/pipe-operator/
	// with compilation fixes.
	template <typename T>
	concept is_expected = requires(T t) 
	{
		typename T::value_type;
		typename T::error_type;
		requires std::is_constructible_v<bool, T>;
		requires std::same_as<std::remove_cvref_t<decltype(*t)>, typename T::value_type>;
		requires std::constructible_from<T, std::unexpected<typename T::error_type>>;
	};

	template <typename T, typename E, typename Function>
	requires std::invocable<Function, T> && is_expected<typename std::invoke_result_t<Function, T>>
	constexpr auto operator|(std::expected<T, E>&& valueOrFail, Function&& f) -> typename std::invoke_result_t<Function, T>
	{
		return valueOrFail
			? std::invoke(std::forward<Function>(f), *std::forward<std::expected<T, E>>(valueOrFail))
			: valueOrFail;
	}
	// Some error types just for the example
	enum class OpErrorType : unsigned char 
	{
		InvalidInput, 
		Overflow, 
		Underflow
	};

	struct Payload 
	{
		std::string fStr{};
		int fVal{};
	};

	// For the pipeline operation - the expected type is Payload,
	// while the 'unexpected' is OpErrorType
	using PayloadOrError = std::expected<Payload, OpErrorType>;

	PayloadOrError Payload_Proc_1(PayloadOrError&& s) 
	{
		if (!s)
			return s;
		++s->fVal;
		s->fStr += "proc by 1,";
		std::println("I'm in Payload_Proc_1, s = '{}'", s->fStr);
		return s;
	}

	PayloadOrError Payload_Proc_2(PayloadOrError&& s) 
	{
		if (!s)
			return s;
		++s->fVal;
		s->fStr += " proc by 2,";
		std::println("I'm in Payload_Proc_2, s = '{}'", s->fStr);
		// Emulate the error, at least once in a while ...
		std::mt19937 rand_gen(std::random_device{} ());
		return (rand_gen() % 2) 
			? s 
			: std::unexpected{ rand_gen() % 2 ? OpErrorType::Overflow : OpErrorType::Underflow };
	}

	PayloadOrError Payload_Proc_3(PayloadOrError&& s) 
	{
		if (!s)
			return s;
		++s->fVal;
		s->fStr += " proc by 3";
		std::println("I'm in Payload_Proc_3, s = '{}'", s->fStr);
		return s;
	}

	void run() 
	{
		auto res = 
			PayloadOrError{ Payload{"Start string ", 42} } 
			| Payload_Proc_1
			| Payload_Proc_2 
			| Payload_Proc_3;

		if (res)
		{
			std::println("Success! Result of the pipe: {}, {}", res->fStr, res->fVal);
			return;
		}

		switch (res.error()) 
		{
			case OpErrorType::InvalidInput:
				std::println("Error: OpErrorType::kInvalidInput");
				break;
			case OpErrorType::Overflow:
				std::println("Error: OpErrorType::kOverflow");
				break;
			case OpErrorType::Underflow:
				std::println("Error: OpErrorType::kUnderflow");
				break;
			default:
				std::println("That's really an unexpected error ...");
				break;
		}
	}
}

namespace M
{
	auto blah(auto&& s) 
	{ 
		return s; 
	}

	struct test
	{
		void b(this auto&& self, int i) {}
	};

	template <typename T, typename Function> requires (std::invocable<Function, T>)
	constexpr auto operator|(T&& t, Function&& f) -> typename std::invoke_result_t<Function, T>
	{
		return std::invoke(std::forward<Function>(f), std::forward<T>(t));
	}

	void run()
	{
		std::vector<int> vec{ 1,2,3 };
		(std::move(vec) | blah<std::vector<int>>);
	}
}

namespace more_pipes
{
	struct p
	{
		auto operator()(auto&& p) { return p; }
	};

	template<typename T, typename C>
	auto operator|(T& v, C&& c)
	{
		return std::invoke(c, v);
	}

	auto run() 
	{
		std::string s;
		s | p{};
	}
}

namespace MoreFixedStrings
{
	template <typename TChar, typename TView, typename TString, size_t N>
	struct ProperFixedString
	{
		using CharType = TChar;
		using ViewType = TView;
		using StringType = TString;

		TChar buf[N]{};

		consteval ProperFixedString(const TChar(&arg)[N]) noexcept
		{
			std::copy_n(arg, N, buf);
		}

		// There's a consteval bug in the compiler.
		// See https://developercommunity.visualstudio.com/t/consteval-function-unexpectedly-returns/10501040
		constexpr operator const TChar* () const noexcept
		{
			return buf;
		}

		consteval TView ToView() const noexcept
		{
			return { buf };
		}

		consteval operator TView() const noexcept
		{
			return { buf };
		}

		constexpr operator TString() const noexcept
		{
			return { buf };
		}

		constexpr TString ToString() const noexcept
		{
			return { buf };
		}
	};
	template<size_t N>
	ProperFixedString(char const (&)[N]) -> ProperFixedString<char, std::string_view, std::string, N>;
	template<size_t N>
	ProperFixedString(wchar_t const (&)[N]) -> ProperFixedString<wchar_t, std::wstring_view, std::wstring, N>;


	constinit ProperFixedString V = "aaaaa";

	template<ProperFixedString F, typename T = decltype(F)>
	struct Y 
	{
		constexpr static T::ViewType Name = F.ToView();
	};

	using FY = Y<L"This is wide!">;
	using FL = Y<"This is narrow!">;

	void Run()
	{
		//FY f1;
		FL f2;
		std::println("{}", f2.Name);
	}
}

int* i = nullptr;

int* GetIt() { return i ? i : (i = new int(1), i); }

struct Reinitialisable
{
	constexpr void Reset() const { Once.~Once(); new (&Once)std::once_flag(); }
	mutable std::once_flag Once;
};


constexpr Reinitialisable R;

void GG() { R.Reset(); }

// https://www.cppstories.com/2021/constexpr-new-cpp20/
template <typename T>
class Buffer {
public:
	constexpr Buffer(size_t n) noexcept : size_(n), mem_(new T[n]) { }
	constexpr ~Buffer() noexcept { delete[] mem_; }

	constexpr Buffer(const Buffer& other) noexcept : size_(other.size_) {
		// ...
	}

	constexpr Buffer(Buffer&& other) noexcept {
		// ...
	}

	constexpr Buffer& operator=(const Buffer& other) noexcept {
		// ...
	}

	constexpr Buffer& operator=(Buffer&& other) noexcept {
		// ...
	}

	constexpr T& operator[](size_t id) noexcept { return mem_[id]; }
	constexpr const T& operator[](size_t id) const noexcept { return mem_[id]; }

	constexpr T* data() const noexcept { return mem_; }
	constexpr size_t size() const noexcept { return size_; }

private:
	T* mem_{ nullptr };
	size_t size_{ 0 };
};

constexpr int naiveSumBuffer(unsigned int n) 
{
	Buffer<int> buf(n); // almost a vector class!
	std::iota(buf.data(), buf.data() + n, 1);
	return std::accumulate(buf.data(), buf.data() + n, 0);
}

constexpr void Something()
{
	
	Reinitialisable r;
}


int main()
{
	StringConversion::Run();
	return 0;

	MoreFixedStrings::Run();

	std::vector vec{ 1,2,3 };
	auto newVec = vec 
		| std::views::filter([](auto&& val) { return val % 2; })
		| std::views::transform([](auto&& val) { return val % 2 ? std::string{ "a" } : std::string{ "b" }; })
		| std::ranges::to<std::vector<std::string>>();
	//can also do std::vector<std::string> sss{ newVec.begin(), newVec.end() };

	vec = vec 
		| std::views::filter([](auto&& val) {return val % 2; }) 
		| std::ranges::to<std::vector<int>>();

	pipes::run();
	pipe_expected::run();

	std::variant<int, char> v;
	Blah(v, 0, 1, 'c');

	MessageLocationTrace m;
	Something(m.Message);
	DeducingThis::Run();
	Dates::Run();

	return 0;
}

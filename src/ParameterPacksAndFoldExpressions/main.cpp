import std;

struct MessageAndLocation
{
	std::string_view message;
	std::source_location loc;

	template<typename T>
	MessageAndLocation(
		T&& msg,
		std::source_location loc = std::source_location::current()
	) requires std::is_convertible_v<T, std::string_view>
		: message{ msg },
		loc{ loc }
	{}
};

template<typename T>
constexpr auto Conversion(const T& value)
{
	if (std::is_convertible_v<T, std::wstring_view>)
	{
		return "A";
	}
	return "B";
}

template<typename...Args>
void TestFunction(MessageAndLocation msg, const Args&... args)
{
	std::string s = std::vformat(msg.message, std::make_format_args(Conversion(args)...));
}

template <typename... Args>
std::vector<std::string> toStringVector(Args... args) {
	std::vector<std::string> result;
	auto initList = { args... };
	using T = typename decltype(initList)::value_type;
	std::vector<T> expanded{initList};
	result.resize(expanded.size());
	std::transform(expanded.begin(), expanded.end(), result.begin(), [](T value) { return std::to_string(value); });
	return result;
}

template<typename...Args>
auto Convert(const Args&... args)
{
	bool x = (std::is_convertible_v<Args&, std::string_view> || ...);




	return std::tuple(args...);
}

template <typename... Args>
	requires (std::is_convertible_v<Args, int> && ...)
void test(Args...) { std::cout << "int\n"; }

template <typename... Args>
concept Istring =
((std::is_convertible_v<Args, int> ||
	std::is_convertible_v<Args, std::string>) && ...) &&
	(std::is_convertible_v<Args, std::string> || ...);

template <Istring... Args>
void test(Args...) { std::cout << "istring\n"; }

int main()
{
	std::wstring something = L"IO";
	TestFunction("Blah {} {} {}", "A", L"A", something);
	auto m = Convert(1, 2, 3, "a");

    return 0;
}

import std;

auto&& To(auto&& v)
{
	//return v;
	return std::forward<decltype(v)>(v);
}

template<typename T>
struct M
{
	M(auto&& f) : T(To(std::forward<decltype(f)>(f))) {}
	T t;
};

int main()
{
	std::string s;
	std::string& t = s;
	const std::string& t2 = s;
	std::println("std::string&: {}", std::same_as<decltype(To(s)), std::string&>);
	std::println("std::string&&: {}", std::same_as<decltype(To(std::string{})), std::string&&>);
	std::println("std::string&: {}", std::same_as<decltype(To(t)), std::string&>);
	std::println("const std::string&: {}", std::same_as<decltype(To(t2)), const std::string&>);

	M m(t2);
}

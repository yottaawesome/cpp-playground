import std;

namespace ReturnTypes
{
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

	void Run()
	{
		std::string s;
		std::string& t = s;
		const std::string& t2 = s;
		std::println("std::string&: {}", std::same_as<decltype(To(s)), std::string&>);
		std::println("std::string&&: {}", std::same_as<decltype(To(std::string{})), std::string&& > );
		std::println("std::string&: {}", std::same_as<decltype(To(t)), std::string&>);
		std::println("const std::string&: {}", std::same_as<decltype(To(t2)), const std::string&>);
	}
}

namespace MoreReturnTypes
{
	// We want to return either std::string& or std::string

	template<typename T>
	struct Return { Return(T) {} };

	template<>
	struct Return<std::string&>
	{
		Return(std::string& s) : S{s} {};
		using Type = std::string&;
		std::string& S;
	};

	template<>
	struct Return<std::string&&>
	{
		using Type = std::string;
		Return(std::string&& s) : S{ std::forward<std::string&&>(s) } {};
		std::string S;
	};

	template<>
	struct Return<std::string_view>
	{
		Return(std::string_view) {}
		using Type = std::string;
	};

	template<>
	struct Return<const char*>
	{
		using Type = std::string;
	};

	template<size_t N>
	struct Return<const char(&)[N]>
	{
		using Type = std::string;
	};

	template<typename T, typename...M>
	concept OneOf = (std::same_as<T, M> or ...);

	template<typename T, typename...M>
	concept ConvertibleTo = (std::convertible_to<T, M> or ...);

	template<typename T>
	concept StringType = ConvertibleTo<std::remove_cvref_t<T>, std::string, std::string_view>;
		//OneOf<std::remove_cvref_t<T>, std::string, std::string_view>;

	decltype(auto) GetString(StringType auto&& value)
	{
		using T = decltype(value);
		if constexpr (OneOf<T, std::string&, const std::string&>)
		{
			return value;
		}
		else if constexpr (std::is_constructible_v<std::string, T>)
		{
			return std::string{ std::forward<T>(value) };
		}
		else
		{
			std::println("b");
			return std::string{};
		}
	}

	template<typename T>
	using AlwaysFalse = std::false_type;


	void Run()
	{
		std::string a;


		//AnyString as(a);
		//std::println("{}", std::same_as<decltype(as.str), std::string&>);

		GetString(a);
		std::println("{}", std::same_as<decltype(GetString(a)), std::string&>);
		//std::println("{}", &GetString(a) == &a);
	}
}

int main()
{
	MoreReturnTypes::Run();
}

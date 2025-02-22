export module randomstuff:stringconversion;
import std;

export namespace StringConversion
{
	template<typename T>
	concept StdString = 
		std::same_as<T, std::string> 
		or std::same_as<T, std::wstring>
		or std::same_as<T, std::string_view>
		or std::same_as<T, std::wstring_view>;

	std::wstring ChangeType(std::string_view)
	{
		return L"wstring";
	}
	std::string ChangeType(std::wstring_view)
	{
		return "string";
	}

	template<StdString TTo>
	decltype(auto) Convert(StdString auto&& from);
	template<StdString TTo>
	decltype(auto) Convert(StdString auto& from);
	template<StdString TTo>
	decltype(auto) Convert(const StdString auto& from);
	template<StdString TTo>
	decltype(auto) Convert(std::string_view from);
	template<StdString TTo>
	decltype(auto) Convert(std::wstring_view from);

	template<StdString T, size_t N>
	decltype(auto) Convert(const char(&from)[N])
	{
		if constexpr (std::same_as<T, std::string>)
		{
			return std::string{ from };
		}
		else
		{
			return ChangeType(from);
		}
	}
	template<StdString T, size_t N>
	decltype(auto) Convert(const wchar_t(&from)[N])
	{
		if constexpr (std::same_as<T, std::wstring>)
		{
			return std::wstring{ from };
		}
		else
		{
			return ChangeType(from);
		}
	}

	// string -> string
	template<>
	decltype(auto) Convert<std::string>(std::string_view from)
	{
		return from;
	}
	template<>
	decltype(auto) Convert<std::string>(std::string& from)
	{
		return from;
	}
	template<>
	decltype(auto) Convert<std::string>(std::string&& from)
	{
		return std::string{ std::move(from) };
	}
	template<>
	decltype(auto) Convert<std::string>(const std::string& from)
	{
		return from;
	}

	// wstring -> wstring
	template<>
	decltype(auto) Convert<std::wstring>(std::wstring_view from)
	{
		return from;
	}
	template<>
	decltype(auto) Convert<std::wstring>(std::wstring& from)
	{
		return from;
	}
	template<>
	decltype(auto) Convert<std::wstring>(std::wstring&& from)
	{
		return std::wstring{ std::move(from) };
	}
	template<>
	decltype(auto) Convert<std::wstring>(const std::wstring& from)
	{
		return from;
	}
	
	// string -> wstring
	template<>
	decltype(auto) Convert<std::wstring>(std::string_view from)
	{
		return ChangeType(from);
	}
	template<>
	decltype(auto) Convert<std::wstring>(std::string& from)
	{
		return ChangeType(from);
	}
	template<>
	decltype(auto) Convert<std::wstring>(std::string&& from)
	{
		return ChangeType(from);
	}
	template<>
	decltype(auto) Convert<std::wstring>(const std::string& from)
	{
		return ChangeType(from);
	}

	// wstring -> string
	template<>
	decltype(auto) Convert<std::string>(std::wstring_view from)
	{
		return ChangeType(from);
	}
	template<>
	decltype(auto) Convert<std::string>(std::wstring& from)
	{
		return ChangeType(from);
	}
	template<>
	decltype(auto) Convert<std::string>(std::wstring&& from)
	{
		return ChangeType(from);
	}
	template<>
	decltype(auto) Convert<std::string>(const std::wstring& from)
	{
		return ChangeType(from);
	}

	void Run()
	{
		std::string s;
		auto& x = Convert<std::string>(s);
		Convert<std::string>("a");
		Convert<std::string>(L"a");
		Convert<std::string>(std::string_view{"a"});
		std::println("{}", &s == &x);
	}
}
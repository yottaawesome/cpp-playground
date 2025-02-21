export module randomstuff:stringconversion;
import std;

export namespace StringConversion
{
	template<typename T>
	concept StdString = std::same_as<T, std::string> or std::same_as<T, std::wstring>;

	std::wstring ChangeType(std::string_view)
	{
		return L"wstring";
	}
	std::string ChangeType(std::wstring_view)
	{
		return "string";
	}

	template<StdString TTo>
	decltype(auto) Convert(auto&& from);

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

	// wstring -> string
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
		std::println("{}", &s == &x);
	}
}
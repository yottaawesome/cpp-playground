export module moreformatting;
import std;
import win32;

namespace Converters
{
	std::string Convert(std::wstring_view wstr)
	{
		if (wstr.empty())
			return {};

		// https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
		// Returns the size in bytes, this differs from MultiByteToWideChar, which returns the size in characters
		const int sizeInBytes = Win32::WideCharToMultiByte(
			Win32::CpUtf8,										// CodePage
			Win32::WcNoBestFitChars,							// dwFlags 
			&wstr[0],										// lpWideCharStr
			static_cast<int>(wstr.size()),					// cchWideChar 
			nullptr,										// lpMultiByteStr
			0,												// cbMultiByte
			nullptr,										// lpDefaultChar
			nullptr											// lpUsedDefaultChar
		);
		if (sizeInBytes == 0)
		{
			const auto lastError = Win32::GetLastError();
			throw std::runtime_error("WideCharToMultiByte() [1] failed");
		}

		std::string strTo(sizeInBytes / sizeof(char), '\0');
		const int status = WideCharToMultiByte(
			Win32::CpUtf8,										// CodePage
			Win32::WcNoBestFitChars,							// dwFlags 
			&wstr[0],										// lpWideCharStr
			static_cast<int>(wstr.size()),					// cchWideChar 
			&strTo[0],										// lpMultiByteStr
			static_cast<int>(strTo.size() * sizeof(char)),	// cbMultiByte
			nullptr,										// lpDefaultChar
			nullptr											// lpUsedDefaultChar
		);
		if (status == 0)
		{
			const auto lastError = Win32::GetLastError();
			throw std::runtime_error("WideCharToMultiByte() [2] failed");
		}

		return strTo;
	}

	std::wstring Convert(std::string_view str)
	{
		if (str.empty())
			return {};

		// https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
		// Returns the size in characters, this differs from WideCharToMultiByte, which returns the size in bytes
		const int sizeInCharacters = Win32::MultiByteToWideChar(
			Win32::CpUtf8,									// CodePage
			0,											// dwFlags
			&str[0],									// lpMultiByteStr
			static_cast<int>(str.size() * sizeof(char)),// cbMultiByte
			nullptr,									// lpWideCharStr
			0											// cchWideChar
		);
		if (sizeInCharacters == 0)
		{
			const auto lastError = Win32::GetLastError();
			throw std::runtime_error("MultiByteToWideChar() [1] failed");
		}

		std::wstring wstrTo(sizeInCharacters, '\0');
		const int status = Win32::MultiByteToWideChar(
			Win32::CpUtf8,									// CodePage
			0,											// dwFlags
			&str[0],									// lpMultiByteStr
			static_cast<int>(str.size() * sizeof(char)),	// cbMultiByte
			&wstrTo[0],									// lpWideCharStr
			static_cast<int>(wstrTo.size())				// cchWideChar
		);
		if (status == 0)
		{
			const auto lastError = Win32::GetLastError();
			throw std::runtime_error("MultiByteToWideChar() [2] failed");
		}

		return wstrTo;
	}
}

export namespace std
{
	// wchar_t -> char
	template<size_t N>
	struct formatter<wchar_t[N], char> : formatter<char, char>
	{
		template <class TContext>
		auto format(const wchar_t(str)[N], TContext&& ctx) const
		{
			return format_to(ctx.out(), "{}", Converters::Convert(str));
		}
	};

	template<>
	struct formatter<const wchar_t*, char> : formatter<char, char>
	{
		template <class TContext>
		auto format(const wchar_t* str, TContext&& ctx) const
		{
			return format_to(ctx.out(), "{}", Converters::Convert(str));
		}
	};

	template<>
	struct formatter<wstring, char> : formatter<char, char>
	{
		template <class TContext>
		auto format(const wstring& str, TContext&& ctx) const
		{
			return format_to(ctx.out(), "{}", Converters::Convert(str));
		}
	};

	template<>
	struct formatter<wstring_view, char> : formatter<char, char>
	{
		template <class TContext>
		auto format(wstring_view str, TContext&& ctx) const
		{
			return format_to(ctx.out(), "{}", Converters::Convert(str));
		}
	};

	// char -> wchar_t
	template<size_t N>
	struct formatter<char[N], wchar_t> : formatter<wchar_t, wchar_t>
	{
		template <class TContext>
		auto format(const char(str)[N], TContext&& ctx) const
		{
			return format_to(ctx.out(), L"{}", Converters::Convert(str));
		}
	};

	template<>
	struct formatter<const char*, wchar_t> : formatter<wchar_t, wchar_t>
	{
		template <class TContext>
		auto format(const char* str, TContext&& ctx) const
		{
			return format_to(ctx.out(), L"{}", Converters::Convert(str));
		}
	};

	template<>
	struct formatter<string, wchar_t> : formatter<wchar_t, wchar_t>
	{
		template <class TContext>
		auto format(const string& str, TContext&& ctx) const
		{
			return format_to(ctx.out(), L"{}", Converters::Convert(str));
		}
	};

	template<>
	struct formatter<string_view, wchar_t> : formatter<wchar_t, wchar_t>
	{
		template <class TContext>
		auto format(string_view str, TContext&& ctx) const
		{
			return format_to(ctx.out(), L"{}", Converters::Convert(str));
		}
	};
}

namespace ConvertingStrings
{
	export void Run()
	{
		auto str = std::format(L"{} {}", "Hello", "world!");
		std::println("{} {}", L"Hello", std::wstring{ L"world!" });
		std::println("{} {}", L"Hello", "world!");
	}
}
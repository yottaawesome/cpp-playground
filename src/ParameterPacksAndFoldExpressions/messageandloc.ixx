module;

#include <Windows.h>

export module messageandloc;
import std;

export namespace Formatting
{
	std::string ConvertString(const std::wstring_view wstr)
	{
		if (wstr.empty())
			return {};

		// https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
		// Returns the size in bytes, this differs from MultiByteToWideChar, which returns the size in characters
		const int sizeInBytes = WideCharToMultiByte(
			CP_UTF8,										// CodePage
			WC_NO_BEST_FIT_CHARS,							// dwFlags 
			&wstr[0],										// lpWideCharStr
			static_cast<int>(wstr.size()),					// cchWideChar 
			nullptr,										// lpMultiByteStr
			0,												// cbMultiByte
			nullptr,										// lpDefaultChar
			nullptr											// lpUsedDefaultChar
		);
		if (sizeInBytes == 0)
			throw std::runtime_error("WideCharToMultiByte() [1] failed");

		std::string strTo(sizeInBytes / sizeof(char), '\0');
		const int status = WideCharToMultiByte(
			CP_UTF8,										// CodePage
			WC_NO_BEST_FIT_CHARS,							// dwFlags 
			&wstr[0],										// lpWideCharStr
			static_cast<int>(wstr.size()),					// cchWideChar 
			&strTo[0],										// lpMultiByteStr
			static_cast<int>(strTo.size() * sizeof(char)),	// cbMultiByte
			nullptr,										// lpDefaultChar
			nullptr											// lpUsedDefaultChar
		);
		if (status == 0)
			throw std::runtime_error("WideCharToMultiByte() [2] failed");

		return strTo;
	}

	std::wstring ConvertString(const std::string_view str)
	{
		if (str.empty())
			return {};

		// https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
		// Returns the size in characters, this differs from WideCharToMultiByte, which returns the size in bytes
		const int sizeInCharacters = MultiByteToWideChar(
			CP_UTF8,									// CodePage
			0,											// dwFlags
			&str[0],									// lpMultiByteStr
			static_cast<int>(str.size() * sizeof(char)),// cbMultiByte
			nullptr,									// lpWideCharStr
			0											// cchWideChar
		);
		if (sizeInCharacters == 0)
			throw std::runtime_error("MultiByteToWideChar() [1] failed");

		std::wstring wstrTo(sizeInCharacters, '\0');
		const int status = MultiByteToWideChar(
			CP_UTF8,									// CodePage
			0,											// dwFlags
			&str[0],									// lpMultiByteStr
			static_cast<int>(str.size() * sizeof(char)),	// cbMultiByte
			&wstrTo[0],									// lpWideCharStr
			static_cast<int>(wstrTo.size())				// cchWideChar
		);
		if (status == 0)
			throw std::runtime_error("MultiByteToWideChar() [2] failed");

		return wstrTo;
	}

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

	struct MessageAndLocationW
	{
		std::wstring_view message;
		std::source_location loc;

		template<typename T>
		MessageAndLocationW(
			T&& msg,
			std::source_location loc = std::source_location::current()
		) requires std::is_convertible_v<T, std::wstring_view>
			: message{ msg },
			loc{ loc }
		{}
	};

	template<typename T>
	inline constexpr auto AutoConvertWideTypes(const T& value)
	{
		if constexpr (std::is_convertible_v<T, std::wstring_view>)
		{
			return ConvertString(value);
		}
		else
		{
			return value;
		}
	}

	template<typename T>
	inline constexpr auto AutoConvertNarrowTypes(const T& value)
	{
		if constexpr (std::is_convertible_v<T, std::string_view>)
		{
			return ConvertString(value);
		}
		else
		{
			return value;
		}
	}

	// This is a combination of the above functions:
	// AutoConvertWideTypes and AutoConvertNarrowTypes.
	// Not really sure whether it's cleaner or not.
	template<bool ToNarrow>
	inline constexpr decltype(auto) AutoConvertStringTypes(auto&& value)
	{
		if constexpr (ToNarrow) // wide-to-narrow
		{
			if constexpr (std::is_convertible_v<decltype(value), std::wstring_view>)
			{
				return ConvertString(value);
			}
			else
			{
				return value;
			}
		}
		else // narrow-to-wide
		{
			if constexpr (std::is_convertible_v<decltype(value), std::string_view>)
			{
				return ConvertString(value);
			}
			else
			{
				return value;
			}
		}
	}

	template<typename...Args>
	void TestPrint(MessageAndLocation msg, Args&&... args)
	{
		std::cout 
			<< std::vformat(
				msg.message, 
				std::make_format_args(
					// Can also use AutoConvertWideTypes(args)...
					AutoConvertStringTypes<true>(args)...
				)) 
			<< std::endl;
	}

	template<typename...Args>
	void TestPrint(MessageAndLocationW msg, Args&&... args)
	{
		std::wcout
			<< std::vformat(
				msg.message,
				std::make_wformat_args(
					// Can also use AutoConvertNarrowTypes(args)...
					AutoConvertStringTypes<false>(args)...
				))
			<< std::endl;
	}

	template<typename...Args>
	auto Convert(const Args&... args)
	{
		bool x = (std::is_convertible_v<Args&, std::string_view> || ...);
		return std::tuple(args...);
	}

	void Print()
	{
		if constexpr (true)
		{
			std::wstring something = L"IO";
			Formatting::TestPrint("Blah {} {} {} {}", 1, "A", L"A", something);
		}
		else
		{
			std::string something = "IO";
			Formatting::TestPrint(L"Blah {} {} {} {}", 1, "A", L"A", something);
		}

		auto m = Convert(1, 2, 3, "a");
	}
}

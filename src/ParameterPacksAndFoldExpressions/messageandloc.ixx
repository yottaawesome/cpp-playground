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

	template<typename...Args>
	void TestPrint(MessageAndLocation msg, const Args&... args)
	{
		std::cout 
			<< std::vformat(
				msg.message, 
				std::make_format_args(
					AutoConvertWideTypes(args)...
				)) 
			<< std::endl;
	}
}

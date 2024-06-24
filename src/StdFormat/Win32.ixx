module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

export module win32;

export namespace Win32
{
	using
		::DWORD,
		::FormatMessageA,
		::MultiByteToWideChar,
		::WideCharToMultiByte,
		::GetLastError
		;

	constexpr auto WcNoBestFitChars = WC_NO_BEST_FIT_CHARS;
	constexpr auto CpUtf8 = CP_UTF8;
}
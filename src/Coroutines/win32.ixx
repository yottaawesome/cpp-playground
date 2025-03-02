module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

export module win32;

template<auto VValue>
struct Constant
{
	operator decltype(VValue)() const noexcept
	{
		return VValue;
	}
};

export namespace Win32
{
	using
		::DWORD,
		::HANDLE,
		::GetStdHandle,
		::SetEvent,
		::CreateEventW,
		::CloseHandle,
		::ResetEvent,
		::WaitForSingleObject
		;

	constexpr Constant<STD_INPUT_HANDLE> StdIn;
	constexpr Constant<STD_OUTPUT_HANDLE> StdOut;
	constexpr Constant<STD_ERROR_HANDLE> StdError;

	constexpr auto Infinite = INFINITE;
}
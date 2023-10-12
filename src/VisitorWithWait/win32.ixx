module;

#include <Windows.h>

export module win32;

export namespace Win32
{
	using ::WaitForSingleObject;
	using ::WaitForSingleObjectEx;
	using ::WaitForMultipleObjects;
	using ::WaitForMultipleObjectsEx;
	using ::CreateEvent;
	using ::HANDLE;
	constexpr auto WaitInfinite = INFINITE;
	using ::SetEvent;

	namespace WaitResult
	{
		constexpr auto Abandoned = WAIT_ABANDONED;
		constexpr auto IoCompletion = WAIT_IO_COMPLETION;
		constexpr auto Wait0 = WAIT_OBJECT_0;
		constexpr auto Timeout = WAIT_TIMEOUT;
		constexpr auto Failed = WAIT_FAILED;
	}
}

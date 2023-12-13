module;

#include <Windows.h>

export module win32;

export namespace Win32
{
	using ::HANDLE;
	using ::DWORD;
	using ::WaitForSingleObject;
	using ::WaitForSingleObjectEx;
	using ::WaitForMultipleObjects;
	using ::WaitForMultipleObjectsEx;
	using ::CreateEvent;
	using ::SetEvent;
	using ::CloseHandle;

	namespace WaitResult
	{
		constexpr auto Abandoned = WAIT_ABANDONED;
		constexpr auto IoCompletion = WAIT_IO_COMPLETION;
		constexpr auto Wait0 = WAIT_OBJECT_0;
		constexpr auto Timeout = WAIT_TIMEOUT;
		constexpr auto Failed = WAIT_FAILED;
	}

	constexpr auto WaitInfinite = INFINITE;
	constexpr auto MaxWaitObjects = MAXIMUM_WAIT_OBJECTS;
}

module;

#include <chrono>
#include <Windows.h>

module nativeapi;

namespace NativeAPI
{
	void XPlatformSleep(const std::chrono::milliseconds ms)
	{
		Sleep(static_cast<DWORD>(ms.count()));
	}
}
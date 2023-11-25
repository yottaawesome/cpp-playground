module;

#include <chrono>
#include <Windows.h>

module nativeapi;

namespace NativeAPI
{
	void XPlatformSleep(const unsigned long ms)
	{
		Sleep(ms);
	}
}
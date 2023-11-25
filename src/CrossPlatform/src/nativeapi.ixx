module;

#include <chrono>

export module nativeapi;

export namespace NativeAPI
{
	void XPlatformSleep(const std::chrono::milliseconds ms);
//	{
//#ifdef __LINUX__
//
//		std::chrono::seconds remS = std::chrono::duration_cast<std::chrono::seconds>(ms);
//		std::chrono::milliseconds remMs = ms - std::chrono::duration_cast<std::chrono::milliseconds>(remS);
//		std::chrono::nanoseconds remNs = std::chrono::duration_cast<std::chrono::nanoseconds>(remMs);
//
//		struct timespec request {
//			.tv_sec = remS.count(),
//				.tv_nsec = remNs.count()
//		};
//		//struct timespec remaining { 0 };
//		if (nanosleep(&request, nullptr))
//			throw std::runtime_error("Sleep failed");
//#else
//		Sleep(static_cast<DWORD>(ms.count()));
//#endif
//	}
}
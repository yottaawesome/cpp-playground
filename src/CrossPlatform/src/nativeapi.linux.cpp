module;

#include <chrono>
#include <stdexcept>
#include <unistd.h>
#include <time.h>

module nativeapi;

// Currently ICEs in gcc-13/g++-13
namespace NativeAPI
{
	void XPlatformSleep(unsigned long ms2)
	{
		std::chrono::milliseconds ms{ ms2 };
		std::chrono::seconds remS = std::chrono::duration_cast<std::chrono::seconds>(ms);
		std::chrono::milliseconds remMs = ms - std::chrono::duration_cast<std::chrono::milliseconds>(remS);
		std::chrono::nanoseconds remNs = std::chrono::duration_cast<std::chrono::nanoseconds>(remMs);

		struct timespec request {
			.tv_sec = remS.count(),
			.tv_nsec = remNs.count()
		};
		//struct timespec remaining { 0 };
		if (nanosleep(&request, nullptr))
			throw std::runtime_error("Sleep failed");
	}
}
#include <format>
#include <iostream>
#include <chrono>
#include <stdexcept>

#ifdef __LINUX__
#include <unistd.h>
#include <time.h>
#else
#include <Windows.h>
#endif

inline constexpr bool IsLinux() noexcept
{
#ifdef __LINUX__
	return true;
#else
	return false;
#endif
}

constexpr const char* Get() noexcept
{
#ifdef __LINUX__
	return "Linux";
#else
	return "Windows";
#endif
}

void XPlatformSleep(const std::chrono::milliseconds ms)
{
#ifdef __LINUX__

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
#else
	Sleep(static_cast<DWORD>(ms.count()));
#endif
}

int main(int argc, char* argv[]) try
{
	const std::chrono::milliseconds ms{ 1000 };
	std::cout << std::format("Sleeping for {} ms\n!", ms.count());
	XPlatformSleep(ms);
	std::cout << std::format("Hello, {}!\n", Get());
	return 0;
}
catch (const std::exception& ex)
{
	std::cerr << std::format("Error: {}!\n", ex.what());
}
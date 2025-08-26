module;

// Required because Microsoft won't fix their fucking bugs in MSVC.
// https://developercommunity.visualstudio.com/t/Thread-local-storage-inside-C-module-l/10529078?sort=newest
#include <toml.hpp>

export module config;
export import :fixedstring;
export import :win32;
export import :concepts;
export import :basicsetting;
export import :config;
export import :toml;
import std;

template<auto VValueType, typename TNative>
struct RegistryValueType
{
	using Native = TNative;
	template<typename T>
	static constexpr bool Same() { return std::same_as<T, TNative>; }
	static constexpr auto Get() noexcept { return VValueType; }
};

template<typename...TTypes>
struct RegistryValueTypes
{
};

constexpr RegistryValueTypes<
	RegistryValueType<Win32::RegistryTypes::Dword, Win32::DWORD>,
	RegistryValueType<Win32::RegistryTypes::String, std::wstring>
> ValueTypes;

auto GetValue(
	auto&& superKey,
	std::wstring_view subKey,
	std::wstring_view name
)
{
}

export extern "C" auto main() -> int
try
{
	Config::Configuration config;
	Config::BasicTomlSetting<std::string, "environment"> setting;
	Config::BasicTomlSetting<std::string, "server", "dev", "ip"> devIP;
	std::println("{}", config.Get(setting));
	std::println("{}", config.Get(devIP));
	return 0;
}
catch (const std::exception& ex)
{
	std::println("Ex: {}", ex.what());
	return 1;
}

#include <toml.hpp>

import std;
import win32;

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

enum class Type
{
	Toml,
	Registry
};

template<size_t N, typename TChar>
struct FixedString
{
	TChar Buffer[N];
	constexpr FixedString(const TChar(&arg)[N])
	{
		std::copy_n(arg, N, Buffer);
	}

	constexpr auto ToView(this auto&& self) noexcept -> std::basic_string_view<TChar, std::char_traits<TChar>>
	{
		return { self.Buffer };
	}

	constexpr auto ToString(this auto&& self) noexcept -> std::basic_string<TChar, std::char_traits<TChar>>
	{
		return { self.Buffer };
	}
};
template<size_t N>
FixedString(const char(&)[N]) -> FixedString<N, char>;
template<size_t N>
FixedString(const wchar_t(&)[N]) -> FixedString<N, wchar_t>;

template<typename TUnderlying, FixedString...VPath>
struct BasicTomlSetting
{
	using Underlying = TUnderlying;
	constexpr static auto Type = Type::Toml;
	constexpr auto Path(this auto&&) noexcept { return std::tuple{VPath...}; }
};

template<typename TUnderlying, FixedString...VPath>
struct BasicRegistrySetting
{
	using Underlying = TUnderlying;
	constexpr static auto Type = Type::Registry;
};

template<typename T>
concept TomlSetting = requires(T t) { requires std::remove_cvref_t<T>::Type == Type::Toml; };
template<typename T>
concept RegistrySetting = requires(T t) { requires std::remove_cvref_t<T>::Type == Type::Registry; };

struct Config
{
	auto Get(TomlSetting auto&& type) -> std::remove_cvref_t<decltype(type)>::Underlying
	{
		using TReturn = std::remove_cvref_t<decltype(type)>::Underlying;

		return std::apply(
			[]<typename...T>(T&&...paths)->TReturn
			{
				return [](this auto&& self, auto&& currentNode, auto&& currentPath, auto&&...paths) -> TReturn
				{
					if constexpr (sizeof...(paths) == 0)
					{
						return toml::find<TReturn>(currentNode, currentPath.ToString());
					}
					else
					{
						return self(currentNode[currentPath.ToString()], paths...);
					}
				}(toml::parse("settings.toml"), std::forward<T>(paths)...);
			}, 
			type.Path()
		);
	}

	auto Get(RegistrySetting auto&& type) -> std::remove_cvref_t<decltype(type)>::Underlying
	{

	}

	auto Save(TomlSetting auto&& type)
	{

	}

	auto Save(RegistrySetting auto&& type)
	{

	}
};

auto main() -> int
try
{
	Config config;
	BasicTomlSetting<std::string, "environment"> setting;
	BasicTomlSetting<std::string, "server", "dev", "ip"> devIP;
	std::println("{}", config.Get(setting));
	std::println("{}", config.Get(devIP));
	return 0;
}
catch (const std::exception& ex)
{
	std::println("Ex: {}", ex.what());
	return 1;
}

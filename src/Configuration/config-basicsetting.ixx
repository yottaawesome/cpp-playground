export module config:basicsetting;
import std;
import :fixedstring;
import :concepts;

export namespace Config
{
	template<typename TUnderlying, FixedString...VPath>
	struct BasicTomlSetting
	{
		using Underlying = TUnderlying;
		constexpr static auto Type = Type::Toml;
		constexpr auto Path(this auto&&) noexcept { return std::tuple{ VPath... }; }
	};

	template<typename TUnderlying, FixedString...VPath>
	struct BasicRegistrySetting
	{
		using Underlying = TUnderlying;
		constexpr static auto Type = Type::Registry;
	};
}

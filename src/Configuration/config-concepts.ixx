export module config:concepts;
import std;

export namespace Config
{
	enum class Type
	{
		Toml,
		Registry
	};

	template<typename T>
	concept TomlSetting = requires(T t) { requires std::remove_cvref_t<T>::Type == Type::Toml; };
	template<typename T>
	concept RegistrySetting = requires(T t) { requires std::remove_cvref_t<T>::Type == Type::Registry; };
}

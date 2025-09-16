export module config:config;
import std;
import :basicsetting;
import :toml;

export namespace Config
{
	struct Configuration
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
}

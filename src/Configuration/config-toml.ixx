module;

#include <toml.hpp>

export module config:toml;
import std;

export namespace toml
{
	using
		::toml::parse,
		::toml::result,
		::toml::basic_value,
		::toml::value,
		::toml::value_t,
		::toml::find,
		::toml::find_or,
		::toml::err,
		::toml::find_or_default,
		::toml::spec
		;
}

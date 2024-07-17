export module a_module;
import std;
import infra;

export
{
	//Still blanks
	//std::string a = [] {return"AAA"; }();
	std::string a = "AAA";

	constexpr Constant<[]() -> std::string { return "AAA"; }> AA;
}

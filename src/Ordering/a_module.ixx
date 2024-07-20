export module a_module;
import std;
import infra;

export
{
	//Still blanks
	//std::string a = [] {return"AAA"; }();
	std::string a = "AAA";

	constexpr Constant<[]() -> std::string { return "AAA"; }> AA;

	Constant2 BB(std::wstring{});
	constexpr Constant2 CC(1);
	//constexpr Constant2 DD([] {});
	//int x = CC;


	constexpr Constant<[]() -> std::string { return AA; }> DD;

	

}

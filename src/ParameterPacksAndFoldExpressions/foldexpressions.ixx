export module foldexpressions;
import std;

export namespace FoldsWithConcepts
{
	template <typename... Args>
		requires (std::is_convertible_v<Args, int> && ...)
	void test(Args...) { std::cout << "int\n"; }

	template <typename... Args>
	concept Istring =
		(
			(
				std::is_convertible_v<Args, int> or
				std::is_convertible_v<Args, std::string>
			) and ...
		)
		and (std::is_convertible_v<Args, std::string> or ...); // at least one string

	template <Istring... Args>
	void test(Args...) { std::cout << "istring\n"; }
}

export namespace PackToVector
{
	template <typename... Args>
	std::vector<std::string> toStringVector(Args... args) 
	{
		std::vector<std::string> result;
		auto initList = { args... };
		using T = typename decltype(initList)::value_type;
		std::vector<T> expanded{ initList };
		result.resize(expanded.size());
		std::transform(
			expanded.begin(), 
			expanded.end(), 
			result.begin(), 
			[](T value) { return std::to_string(value); }
		);
		return result;
	}
}
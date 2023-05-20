/*
* Useful references:
* https://www.modernescpp.com/index.php/smart-tricks-with-fold-expressions
* https://gist.github.com/alepez/de533a78acf5a1079a04
* https://www.foonathan.net/2020/05/fold-tricks/
* https://subscription.packtpub.com/book/programming/9781787120495/1/ch01lvl1sec16/implementing-handy-helper-functions-with-fold-expressions
* https://stackoverflow.com/questions/46806239/test-if-all-elements-are-equal-with-c17-fold-expression
* https://www.fluentcpp.com/2019/01/25/variadic-number-function-parameters-type/
* https://www.fluentcpp.com/2019/01/29/how-to-define-a-variadic-number-of-arguments-of-the-same-type-part-2/
* https://www.fluentcpp.com/2019/02/05/how-to-define-a-variadic-number-of-arguments-of-the-same-type-part-3/
* https://www.fluentcpp.com/2020/01/07/how-to-define-a-variadic-number-of-arguments-of-the-same-type-part-4/
* https://www.fluentcpp.com/2021/06/07/how-to-define-a-variadic-number-of-arguments-of-the-same-type-part-5/
* https://stackoverflow.com/questions/70035099/how-to-extract-requires-clause-with-a-parameter-pack-whose-parameters-are-relate
* https://www.modernescpp.com/index.php/from-variadic-templates-to-fold-expressions
* https://www.fluentcpp.com/2021/03/12/cpp-fold-expressions/
* https://en.cppreference.com/w/cpp/language/fold
*/

import std;
import messageandloc;

template <typename... Args>
std::vector<std::string> toStringVector(Args... args) {
	std::vector<std::string> result;
	auto initList = { args... };
	using T = typename decltype(initList)::value_type;
	std::vector<T> expanded{initList};
	result.resize(expanded.size());
	std::transform(expanded.begin(), expanded.end(), result.begin(), [](T value) { return std::to_string(value); });
	return result;
}

template<typename...Args>
auto Convert(const Args&... args)
{
	bool x = (std::is_convertible_v<Args&, std::string_view> || ...);
	return std::tuple(args...);
}

template <typename... Args>
	requires (std::is_convertible_v<Args, int> && ...)
void test(Args...) { std::cout << "int\n"; }

template <typename... Args>
concept Istring =
((std::is_convertible_v<Args, int> ||
	std::is_convertible_v<Args, std::string>) && ...) &&
	(std::is_convertible_v<Args, std::string> || ...);

template <Istring... Args>
void test(Args...) { std::cout << "istring\n"; }

int main()
{
	if constexpr (true)
	{
		std::wstring something = L"IO";
		Formatting::TestPrint("Blah {} {} {} {}", 1, "A", L"A", something);
	}
	else
	{
		std::string something = "IO";
		Formatting::TestPrint(L"Blah {} {} {} {}", 1, "A", L"A", something);
	}

	auto m = Convert(1, 2, 3, "a");

    return 0;
}

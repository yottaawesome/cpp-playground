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
	// Adapted from https://gist.github.com/alepez/de533a78acf5a1079a04
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

export namespace NiftyExpressions
{
	// Adapted from https://www.foonathan.net/2020/05/fold-tricks/
	template <typename H, typename ... T>
	auto add(H head, T... tail)
	{
		return (head + ... + tail);
		// expands to: head + tail[0] + tail[1] + ...
	}

	void f(auto&&) {}

	void CallFunctionWithEachElement(auto&&...ts)
	{
		(f(ts), ...);
	}

	void CallFunctionWithEachElementReverse(auto&&...ts)
	{
		int dummy;
		(dummy = ... = (f(ts), 0));
	}

	void CallFuncWithEachElementUntilPredicateMatches(auto&& pred, auto&&...ts)
	{
		((pred(ts) ? false : (f(ts), true)) && ...);
		// expands to: (pred(ts[0]) ? false : (f(ts[0]), true))
		//              && (pred(ts[1]) ? false : (f(ts[1]), true))
		//              && ...
	}

	void AnyElementMatchesAPredicate(auto&& pred, auto&&...ts)
	{
		bool any_of = (pred(ts) || ...);
		// expands to: pred(ts[0]) || pred(ts[1]) || ...
	}

	void HowManyMatchPredicate(auto&& pred, auto&&...ts)
	{
		auto count = (std::size_t(0) + ... + (pred(ts) ? 1 : 0));
		// expands to: std::size_t(0) + (pred(ts[0]) ? 1 : 0)
		//                            + (pred(ts[1]) ? 1 : 0)
		//                            + ...
	}

	void FirstElementThatMatchesPredicate(auto&& pred, auto&&...ts)
	{
		std::common_type_t<decltype(ts)...> result;
		bool found = ((pred(ts) ? (result = ts, true) : false) || ...);
		// expands to: (pred(ts[0]) ? (result = ts[0], true) : false)
		//          || (pred(ts[1]) ? (result = ts[1], true) : false)
		//          || ...
	}

	void GetNthElement(int n, auto&&... ts)
	{
		std::common_type_t<decltype(ts)...> result;
		std::size_t i = 0;
		((i++ == n ? (result = ts, true) : false) || ...);
	}

	void GetFirstElement(auto&&... ts)
	{
		std::common_type_t<decltype(ts)...> result;
		((result = ts, true) || ...);
		// expands to: (result = ts[0], true)
		//          || (result = ts[1], true)
		//          || ...
	}

	void GetLastElement(auto&&... ts)
	{
		auto result = (ts, ...);
		// expands to: ts[0], ts[1], ...
	}

	void GetMinimal(auto&&... ts)
	{
		auto min = (ts, ...);
		((ts < min ? min = ts, 0 : 0), ...);
		// expands to: (ts[0] < min ? min = ts[0], 0 : 0),
		//             (ts[1] < min ? min = ts[1], 0 : 0),
		//             ...
	}

	void Run()
	{
		NiftyExpressions::CallFunctionWithEachElement(1, 2);
		NiftyExpressions::CallFunctionWithEachElementReverse(1, 2);
	}
}
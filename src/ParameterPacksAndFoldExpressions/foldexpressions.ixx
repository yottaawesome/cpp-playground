export module foldexpressions;
import std;

export namespace FoldsWithConcepts
{
	// https://stackoverflow.com/a/46809861/7448661
	template <class... Args>
	constexpr bool all_equal(Args const&... args) {
		if constexpr (sizeof...(Args) == 0)
			return true;
		else return 
			[](auto const& a0, auto const&... rest) 
			{
				return ((a0 == rest) && ...);
			}(args...);
	}

	// https://stackoverflow.com/questions/70035099/how-to-extract-requires-clause-with-a-parameter-pack-whose-parameters-are-relate
	template <typename... Args>
		requires (std::is_convertible_v<Args, int> && ...)
	void test1(Args...) { std::cout << "int\n"; }

	template <typename... Args>
	concept Istring = (
		(
			std::is_convertible_v<Args, int>
			or std::is_convertible_v<Args, std::string>
		) and ...
	) and (std::is_convertible_v<Args, std::string> or ...); // at least one string

	template <typename... Args>
		requires Istring<Args...> // need to do this to check all parameters
	void test2(Args...) { std::cout << "istring\n"; }

	void Run()
	{

		test2(1, std::string{});
	}
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

export namespace MoreNiftyFolds
{
	// Adapted from https://www.modernescpp.com/index.php/smart-tricks-with-fold-expressions/
	template<typename ... Args>
	void printMe(Args&& ... args) 
	{
		(std::cout << ... << std::forward<Args>(args)) << '\n';
	}

	template<typename T, typename... Args>
	void myPushBack(std::vector<T>& v, Args&&... args) 
	{
		(v.push_back(args), ...);
	}

	template<typename ... Ts>
	struct Overload : Ts ... 
	{
		using Ts::operator() ...;
	};

	// https://www.modernescpp.com/index.php/from-variadic-templates-to-fold-expressions
	bool allVar()
	{
		return true;
	}

	template<typename T, typename ...Ts>
	bool allVar(T t, Ts ... ts) 
	{
		return t && allVar(ts...);
	}

	template<typename... Args>
	bool all(Args... args) 
	{ 
		return (... && args); 
	}

	// diffL1(1, 2, 3) -> (1 - 2) - 3
	template<typename... Args>
	auto diffL1(Args const&... args) 
	{
		return (... - args);
	}

	// diffR1(1, 2, 3) -> 1 - (2 - 3)
	template<typename... Args>
	auto diffR1(Args const&... args) 
	{
		return (args - ...);
	}

	// diffL(10, 1, 2, 3) -> ((10 - 1) - 2) - 3
	template<typename Init, typename... Args>
	auto diffL(Init init, Args const&... args) 
	{
		return (init - ... - args);
	}

	// diffR(10, 1, 2, 3) -> 1 - (2 - (3 - 10))
	template<typename Init, typename... Args>
	auto diffR(Init init, Args const&... args) 
	{
		return (args - ... - init);
	}

	void Run()
	{
		auto TypeOfIntegral = Overload
		{
			[](int)				{ return "int"; },
			[](unsigned int)	{ return "unsigned int"; },
			[](long int)		{ return "long int"; },
			[](long long int)	{ return "long long int"; },
			[](auto)			{ return "unknown type"; },
		};

		std::tuple t{ 1, 1.f };

		std::apply(
			[&TypeOfIntegral](auto&& t) { std::cout << TypeOfIntegral(t); }, 
			std::forward_as_tuple(t)
		);
	}
}

export namespace AllTheSameType
{
	// https://www.fluentcpp.com/2019/01/25/variadic-number-function-parameters-type/
	// https://www.fluentcpp.com/2019/01/29/how-to-define-a-variadic-number-of-arguments-of-the-same-type-part-2/
	// https://www.fluentcpp.com/2019/02/05/how-to-define-a-variadic-number-of-arguments-of-the-same-type-part-3/
	// https://www.fluentcpp.com/2020/01/07/how-to-define-a-variadic-number-of-arguments-of-the-same-type-part-4/
	// https://www.fluentcpp.com/2021/06/07/how-to-define-a-variadic-number-of-arguments-of-the-same-type-part-5/
	// https://www.fluentcpp.com/2021/03/12/cpp-fold-expressions/
	template<typename T, typename... Ts>
	using AllSame = std::enable_if_t<std::conjunction_v<std::is_same<T, Ts>...>>;

	template<typename T, typename... Ts, typename = AllSame<T, Ts...>>
	void f1(T const& value, const Ts&... values) {}

	// Better way of doing the above
	// With folds (not necessary)
	template<typename T, typename...TArgs>
	concept AllTheSame1 = (std::is_same_v<TArgs, T> and ...);

	// Without folds
	template<typename T, typename TArgs>
	concept AllTheSame2 = std::is_same_v<TArgs, T>;

	// AllTheSame1 and AllTheSame2 both work
	void f2(AllTheSame2<std::string> auto&&... args) {}

	void Run()
	{
		f2(std::string{}, std::string{});
	}
}
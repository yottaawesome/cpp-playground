export module formatting;
import std;
import win32;

export namespace Formats
{
	// https://stackoverflow.com/a/77635205/7448661
	template<typename... Args>
	inline auto lazy_format(std::format_string<Args...> fmt, Args&&... args)
	{
		return
			[fmt, args_tuple = std::tuple<Args...>(std::forward<Args>(args)...)]() mutable
			{
				return std::apply(
					[fmt](auto&... args)
					{
						return std::format(fmt, std::forward<Args>(args)...);
					},
					args_tuple
				);
			};
	}

	// Constant expression, can use regular format
	template<typename... Args>
	inline std::string DoFormat1(std::format_string<Args...> fmt, Args&&... args)
	{
		return std::format(fmt, std::forward<Args>(args)...);
	}

	// Not a constant expression, requires vformat
	template<typename... Args>
	inline std::string DoFormat2(std::string_view fmt, Args&&... args)
	{
		return std::format(fmt, std::forward<Args>(args)...);
	}

	void str(std::string s)
	{
		DoFormat1("Hey {}", s);
		DoFormat1("Hey");
	}

	void Run()
	{
		str("Haha");
	}
}

// Adapted from https://www.cppstories.com/2023/fun-print-tables-format/
export namespace PrintTables
{
	template <typename T>
	size_t MaxKeyLength1(const std::map<std::string, T>& m)
	{
		auto res = std::ranges::max_element(
			m, 
			[](const auto& a, const auto& b) 
			{
				return a.first.length() < b.first.length();
			});
		return res->first.length();
	}

	template <typename T>
	size_t MaxKeyLength(const std::map<std::string, T>& m) 
	{
		auto res = std::ranges::max_element(
			std::views::keys(m), 
			[](const auto& a, const auto& b) 
			{
				return a.length() < b.length();
			});
		return (*res).length();
	}

	void Run()
	{
		using namespace std::chrono;

		constexpr size_t NumRows = 5;
		std::chrono::year_month_day startDate{ 2023y, month{February}, 20d };
		const std::map<std::string, std::array<double, NumRows>> productsToOrders{
			{ "apples", {100, 200, 50.5, 30, 10}},
			{ "bananas", {80, 10, 100, 120, 70}},
			{ "carrots", {130, 75, 25, 64.5, 128}},
			{ "tomatoes", {70, 100, 170, 80, 90}}
		};

		const auto ColLength = MaxKeyLength(productsToOrders) + 2;

		// print headers:
		std::cout << std::format("{:>{}}", "date", ColLength);
		for (const auto& name : std::views::keys(productsToOrders))
			std::cout << std::format("{:>{}}", name, ColLength);
		std::cout << '\n';

		// print values:
		for (size_t i = 0; i < NumRows; ++i) 
		{
			const auto nextDay = std::chrono::sys_days{ startDate } + days{ i };
			std::cout << std::format("{:>{}}", nextDay, ColLength);
			for (const auto& values : std::views::values(productsToOrders)) 
			{
				std::cout << std::format("{:>{}.2f}", values[i], ColLength);
			}
			std::cout << '\n';
		}
	}
}

// Adapted from https://www.cppstories.com/2022/custom-stdformat-cpp20/ 
export namespace CustomTypesCpp20
{
	struct Index
	{
		unsigned int id_{ 0 };
	};

	struct Color 
	{
		uint8_t r{ 0 };
		uint8_t g{ 0 };
		uint8_t b{ 0 };
	};

	struct Color2
	{
		uint8_t r{ 0 };
		uint8_t g{ 0 };
		uint8_t b{ 0 };
	};
}

// Cannot be defined in a namespace
template <>
struct std::formatter<CustomTypesCpp20::Index> : std::formatter<int>
{
	// for debugging only
	//formatter() { std::cout << "formatter<Index>()\n"; }

	/*constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}*/

	auto format(const CustomTypesCpp20::Index& id, std::format_context& ctx) const
	{
		return std::formatter<int>::format(id.id_, ctx);
	}
};

template <>
struct std::formatter<CustomTypesCpp20::Color> : std::formatter<string_view> 
{
	auto format(const CustomTypesCpp20::Color& col, std::format_context& ctx) const 
	{
		std::string temp;
		std::format_to(
			std::back_inserter(temp), 
			"({}, {}, {})",
			col.r, 
			col.g, 
			col.b
		);
		return std::formatter<string_view>::format(temp, ctx);
	}
};

template <>
struct std::formatter<CustomTypesCpp20::Color2> 
{
	constexpr auto parse(std::format_parse_context& ctx) 
	{
		auto pos = ctx.begin();
		while (pos != ctx.end() && *pos != '}') 
		{
			if (*pos == 'h' || *pos == 'H')
				isHex_ = true;
			++pos;
		}
		return pos;  // expect `}` at this position, otherwise, 
		// it's error! exception!
	}

	auto format(const CustomTypesCpp20::Color2& col, std::format_context& ctx) const 
	{
		if (isHex_) 
		{
			uint32_t val = col.r << 16 | col.g << 8 | col.b;
			return std::format_to(ctx.out(), "#{:x}", val);
		}

		return std::format_to(ctx.out(), "({}, {}, {})", col.r, col.g, col.b);
	}

	bool isHex_{ false };
};

export namespace CustomTypesCpp20
{
	void Run()
	{
		auto str = std::format("{}", Index{});
	}
}

export namespace SomeBullshitFormatting
{
	struct Formatted
	{
		template<typename...Args>
		Formatted(
			std::format_string<Args...> fmt,
			Args&&...args
		) : message{ std::format(fmt, std::forward<Args>(args)...) }
		{ }

		std::string message;
	};

	struct Loc
	{
		std::source_location loc;
		Loc(const std::source_location l = std::source_location::current())
			: loc(l)
		{ }
	};

	template<typename...Args>
	struct Container
	{
		std::format_string<Args...> m_fmt;
		constexpr Container(
			std::format_string<Args...> fmt,
			std::source_location l = std::source_location::current()
		) : m_fmt(fmt)
		{

		}
	};

	struct MessageAndLocation2
	{
		std::string message;
		std::source_location location = std::source_location::current();

		template<typename...Args>
		constexpr MessageAndLocation2(
			Container<Args...> fmt,
			Args&&...args
		) : message{ std::format(fmt.m_fmt, std::forward<Args>(args)...) }
		{
			//message = fmt.get();
		}
	};

	template<typename...Args>
	void Fmt1(
		Formatted&& msg,
		const std::source_location& loc = std::source_location::current()
	)
	{ }

	void Fmt2(MessageAndLocation2 msg)
	{
		std::cout << msg.location.line() << std::endl;
	}

	template<typename... Args>
	[[no_discard]] inline auto Formatter(std::format_string<Args...> fmt, Args&&... args)
	{
		return
			[fmt, args_tuple = std::tuple<Args...>(std::forward<Args>(args)...)](
				const std::source_location& l = std::source_location::current()
			) mutable
			{
				return std::apply(
					[fmt](auto&... args)
					{
						return std::format(fmt, std::forward<Args>(args)...);
					},
					args_tuple
				);
			};
	}

	template<typename F, typename... Args>
	[[nodiscard]] inline auto Deferred(F func, Args&&... args)
	{
		return
			[func, args_tuple = std::tuple<Args...>(std::forward<Args>(args)...)](
				const std::source_location& l = std::source_location::current()
			) mutable
			{
				return std::apply(
					func,
					args_tuple
				);
			};
	}

	void SomeFunction(int i)
	{

	}

	void Run()
	{
		Deferred(SomeFunction, 1)();

		Formatter("A {}", 1)();
		Fmt1({ "{}", 1 });
		MessageAndLocation2({ "{}" }, 1);
		Fmt2({ {"{}"}, 1 });
		Formatted("{}", 1);
	}
}

// See also https://www.cppstories.com/2020/02/extra-format-cpp20.html/
export namespace FormattingTo
{
	template<typename...TArgs>
	std::string FormatToString(std::format_string<TArgs...> fmt, TArgs&&...args)
	{
		std::string buffer;
		std::format_to(
			std::back_inserter(buffer),
			fmt,
			std::forward<TArgs>(args)...);
		return buffer;
	}

	void Run()
	{
		std::string buffer;

		std::format_to(
			std::back_inserter(buffer), //< OutputIt
			"Hello, C++{}!\n",          //< fmt 
			"20");                      //< arg
		std::cout << buffer;
		buffer.clear();

		std::format_to(
			std::back_inserter(buffer), //< OutputIt
			"Hello, {0}::{1}!{2}",      //< fmt 
			"std",                      //< arg {0}
			"format_to()",              //< arg {1}
			"\n",                       //< arg {2}
			"extra param(s)...");       //< unused
		std::cout << buffer;

		std::wstring wbuffer;
		std::format_to(
			std::back_inserter(wbuffer),//< OutputIt 
			L"Hello, {2}::{1}!{0}",     //< fmt
			L"\n",                      //< arg {0}
			L"format_to()",             //< arg {1}
			L"std",                     //< arg {2}
			L"...is not..."             //< unused
			L"...an error!");           //< unused
		std::wcout << wbuffer;
	}
}



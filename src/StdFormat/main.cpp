import formatting;
//import converters;

struct F
{
	int X;
};

template<>
struct std::formatter<F, char> : std::formatter<char, char>
{
	template <class _FormatContext>
	auto format(const F& str, _FormatContext&& ctx) const
	{
		return format_to(ctx.out(), "{}", str.X);
	}
};

template<>
struct std::formatter<F, wchar_t> : std::formatter<wchar_t, wchar_t>
{
	template <class _FormatContext>
	auto format(const F& str, _FormatContext&& ctx) const
	{
		return format_to(ctx.out(), L"{}", str.X);
	}
};

auto main() -> int
{
	std::format(L"{}", F{ 1 });
	std::format("{}", F{1});

	//PrintTables::Run();
	//FormattingTo::Run();
	//SomeBullshitFormatting::Run();
	return 0;
}
export module config:fixedstring;
import std;

export namespace Config
{
	template<size_t N, typename TChar>
	struct FixedString
	{
		TChar Buffer[N];
		constexpr FixedString(const TChar(&arg)[N])
		{
			std::copy_n(arg, N, Buffer);
		}

		constexpr auto ToView(this auto&& self) noexcept -> std::basic_string_view<TChar, std::char_traits<TChar>>
		{
			return { self.Buffer };
		}

		constexpr auto ToString(this auto&& self) noexcept -> std::basic_string<TChar, std::char_traits<TChar>>
		{
			return { self.Buffer };
		}
	};
	template<size_t N>
	FixedString(const char(&)[N]) -> FixedString<N, char>;
	template<size_t N>
	FixedString(const wchar_t(&)[N]) -> FixedString<N, wchar_t>;
}

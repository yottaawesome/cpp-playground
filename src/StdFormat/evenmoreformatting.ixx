export module evenmoreformatting;
import std;

export namespace EvenMoreFormatting
{
	struct SomeStructToFormat
	{
		int SomeField = 10;
		std::string SomeName = "Blah";
	};
}

// Probably a good idea to define these formatters in the same file as the types they format.
export namespace std
{
	template<>
	struct formatter<EvenMoreFormatting::SomeStructToFormat> : std::formatter<char>
	{
		template<typename TContext>
		auto format(const EvenMoreFormatting::SomeStructToFormat& object, TContext&& ctx) const
		{
			return format_to(ctx.out(), "{}:{}", object.SomeField, object.SomeName);
		}
	};
}
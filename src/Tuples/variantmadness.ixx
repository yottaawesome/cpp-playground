export module variantmadness;
import std;

export namespace RuntimeSetting
{
	namespace Helper
	{
		template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
	}

	struct TagA {};
	struct TagB {};
	struct TagC {};

	template <typename TVariant, size_t N = 0>
	void RuntimeSet(TVariant& tup, size_t idx)
	{
		if (N == idx)
			tup = std::variant_alternative_t<N, TVariant>{};
		if constexpr (N + 1 < std::variant_size_v<TVariant>)
			RuntimeSet<TVariant, N + 1>(tup, idx);
	}

	void Run()
	{
		std::variant<TagA, TagB, TagC> variant;
		RuntimeSet(variant, 1);
		TagB f = std::get<1>(variant);

		std::visit(
			Helper::overload{
				[](const TagA& tag)
				{
					std::cout << "TagA...\n";
				},
				[](const TagB& tag)
				{
					std::cout << "TagB...\n";
				},
				[](const TagC& tag)
				{
					std::cout << "TagC...\n";
				}
			},
			variant
		);
	}
}
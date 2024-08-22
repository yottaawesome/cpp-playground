export module infra;
import std;

export
{
	template<auto VConstant>
	struct Constant
	{
		using TConstant = std::invoke_result_t<decltype(VConstant)>;
		
		constexpr operator const TConstant&() const noexcept
		{
			return Get();
		}

		constexpr const TConstant& Get() const noexcept
		{
			std::call_once(
				m_flag,
				[](std::unique_ptr<TConstant>& cache) { cache = std::make_unique<TConstant>(std::invoke(VConstant)); },
				std::ref(m_cache)
			);
			return *m_cache;
		}

		private:
		mutable std::unique_ptr<TConstant> m_cache;
		mutable std::once_flag m_flag;
	};

	template<typename T>
	struct Constant2
	{
		constexpr Constant2(T&& t) : m_t(t) {}
		/*constexpr operator const T& () const noexcept
		{
			return m_t;
		}*/

		/*operator std::invoke_result_t<T>() const noexcept
			requires std::invocable<T>
		{
			return std::invoke(m_t);
		}*/

		T m_t;
	};
}
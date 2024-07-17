export module infra;
import std;

export
{
	template<auto VConstant>
	struct Constant
	{
		using T = std::invoke_result_t<decltype(VConstant)>;
		
		constexpr ~Constant()
		{
			if (m_cache)
				delete m_cache;
		}

		constexpr operator T() const 
		{
			std::call_once(
				m_flag, 
				[](T** cache) { *cache = new T{ std::invoke(VConstant) }; }, 
				&m_cache
			);
			return *m_cache; 
		}

		mutable T* m_cache = nullptr;
		mutable std::once_flag m_flag;
	};
}
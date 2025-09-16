module;

#include <Windows.h>

export module config:win32;

export namespace Win32
{
	template<auto VValue>
	struct Constant
	{
		constexpr auto Get(this auto&&) noexcept -> decltype(VValue) { return VValue; }
	};

	using
		::LRESULT,
		::DWORD,
		::WCHAR,
		::PWCHAR,
		::GetLastError,
		::RegOpenKeyW,
		::RegCloseKey,
		::RegGetValueW
		;

	namespace RegistryTypes
	{
		enum
		{
			Dword = RRF_RT_DWORD,
			String = RRF_RT_REG_SZ
		};
	}
	Constant<HKEY_CURRENT_USER> HKLU;
}

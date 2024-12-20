import std;

namespace OutPtrWithCustomPtr
{
	// conforms to the pointer concept
	struct PtrType
	{
		using element_type = int;

		constexpr ~PtrType() 
		{ 
			reset(); 
		}
		constexpr PtrType() noexcept = default;
		constexpr PtrType(int* i) noexcept : m_i(i) {}

		PtrType(const PtrType&) = delete;
		PtrType& operator=(const PtrType&) = delete;
		PtrType(PtrType&& other) noexcept 
		{ 
			Move(other); 
		}
		PtrType& operator=(PtrType&& other) noexcept 
		{ 
			return Move(other); 
		}

		constexpr void reset() noexcept
		{ 
			delete m_i; 
			m_i = nullptr; 
		}

		private:
		PtrType& Move(PtrType& other) noexcept
		{
			reset();
			m_i = other.m_i;
			other.m_i = nullptr;
			return *this;
		}

		int* m_i = nullptr;
	};

	void FnWithOutParam(int** i)
	{
		*i = new int(1);
	}

	void FnWithOutParam2(int** i)
	{
		**i = 10;
	}

	auto Run() -> void
	{
		PtrType ptr;
		FnWithOutParam(std::out_ptr(ptr));
		// Same as above, but with std pointer
		std::unique_ptr<int> ptr2;
		FnWithOutParam(std::out_ptr(ptr2));

		std::unique_ptr<int> ptr3 = std::make_unique<int>(5);
		FnWithOutParam2(std::inout_ptr(ptr3));
	}
}

int main()
{
	OutPtrWithCustomPtr::Run();
	return 0;
}

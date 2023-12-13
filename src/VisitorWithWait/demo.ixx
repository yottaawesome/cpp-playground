export module demo;
import std;
import win32;

export namespace WithTuple
{
	class EventA
	{
		public:
			Win32::HANDLE Handle;
			void operator()()
			{
				std::cout << "Event A invoked...\n";
			}
	};

	class EventB
	{
		public:
			Win32::HANDLE Handle;
			void operator()()
			{
				std::cout << "Event B invoked...\n";
			}
	};

	// https://stackoverflow.com/a/8866219
	template <class T, class M> M GetMemberType(M T::*) {}

	template<typename T>
	concept EventInvocable = requires(T t)
	{
		//using x = decltype(get_member_type(&T::Handle));
		requires std::is_same_v<decltype(GetMemberType(&T::Handle)), Win32::HANDLE>;
		t();
	};

	// Three ways of constrained function signatures
	EventInvocable auto SomeFunc1();
	auto SomeFunc2() -> EventInvocable auto;
	EventInvocable auto SomeFunc3() -> auto;

	template <class Tuple, size_t N = 0>
	auto RuntimeInvoke(Tuple& tup, size_t idx)
	{
		if (N == idx)
		{
			return std::get<N>(tup)();
		}

		if constexpr (N + 1 < std::tuple_size_v<Tuple>)
		{
			return RuntimeInvoke<Tuple, N + 1>(tup, idx);
		}
	}

	template<EventInvocable...E>
	class EventHandler
	{
		public:
			EventHandler(E&...args)
				: m_handles{ std::forward<E>(args)... }
			{
				std::apply(
					[this](auto&&... val)
					{
						m_winhandles = std::array{ std::forward<Win32::HANDLE>(val.Handle) ... };
					},
					m_handles
				);
			}

		public:
			void Wait()
			{
				auto index = Win32::WaitForMultipleObjects(
					static_cast<DWORD>(m_winhandles.size()),
					m_winhandles.data(),
					false,
					Win32::WaitInfinite
				);
				if (index >= m_winhandles.size())
					throw std::runtime_error(std::format("Error waiting on object {}", index));
				RuntimeInvoke(m_handles, index);
			}

		private:
			std::tuple<E...> m_handles;
			std::array<Win32::HANDLE, sizeof...(E)> m_winhandles;
	};

	int Run()
	{
		Win32::HANDLE h_a = Win32::CreateEventW(
			nullptr,
			false,
			false,
			nullptr
		);
		if (not h_a)
			return 1;
		using HandlePtr = std::unique_ptr<std::remove_pointer_t<Win32::HANDLE>, decltype(&Win32::CloseHandle)>;
		HandlePtr ptr_a(h_a, &Win32::CloseHandle);

		Win32::HANDLE h_b = Win32::CreateEventW(
			nullptr,
			false,
			false,
			nullptr
		);
		if (not h_b)
			return 2;
		HandlePtr ptr_b(h_b, &Win32::CloseHandle);

		EventA a{
			.Handle = h_a
		};
		EventB b{
			.Handle = h_b
		};

		EventHandler e(a, b);
		Win32::SetEvent(h_a);
		e.Wait();
		Win32::SetEvent(h_b);
		e.Wait();

		return 0;
	}
}

export namespace WithTuple2
{
	namespace Helper
	{
		template<class... Ts> 
		struct Overload : Ts... 
		{
			using Type = Overload<Ts...>;
			static constexpr auto Size = sizeof...(Ts);
			using Ts::operator()...; 
		};
	}

	namespace State
	{
		struct WaitA {};
		struct WaitB {};
		struct Timeout {};
		struct Failed {};
		struct Abandoned {};
		struct IOCompletion {};
	}

	using WaitState = std::variant<
		State::WaitA, 
		State::WaitB, 
		State::Timeout,
		State::Failed,
		State::Abandoned,
		State::IOCompletion
	>;

	struct HandleDeleter
	{
		void operator()(Win32::HANDLE h)
		{
			Win32::CloseHandle(h);
		}
	};
	using HandleUniquePtr = std::unique_ptr<std::remove_pointer_t<Win32::HANDLE>, HandleDeleter>;

	template <typename TVariant, size_t N = 0>
	void RuntimeSet(TVariant& tup, size_t idx)
	{
		if (N == idx)
			tup = std::variant_alternative_t<N, TVariant>{};
		if constexpr (N + 1 < std::variant_size_v<TVariant>)
			RuntimeSet<TVariant, N + 1>(tup, idx);
	}

	constexpr std::chrono::milliseconds InfiniteWait{Win32::WaitInfinite};

	void WaitOn(
		WaitState& state, 
		const std::vector<Win32::HANDLE>& handles,
		const std::chrono::milliseconds waitTime
	)
	{
		Win32::DWORD index = Win32::WaitForMultipleObjects(
			static_cast<Win32::DWORD>(handles.size()), 
			handles.data(), 
			false, 
			static_cast<Win32::DWORD>(waitTime.count())
		);
		switch (index)
		{
			case Win32::WaitResult::Abandoned:
				state = State::Abandoned{};
				break;
			case Win32::WaitResult::IoCompletion:
				state = State::IOCompletion{};
				break;
			case Win32::WaitResult::Timeout:
				state = State::Timeout{};
				break;
			case Win32::WaitResult::Failed:
				state = State::Failed{};
				break;
			default:
				RuntimeSet(state, index);
		}
	}

	void Do(const State::WaitA& tag)
	{
		std::cout << "WaitA...\n";
	}
	void Do(const State::WaitB& tag)
	{
		std::cout << "WaitB...\n";
	}
	void Do(const State::Timeout& tag)
	{
		std::cout << "Timeout...\n";
	}
	void Do(const State::Failed& tag)
	{
		std::cout << "Failed...\n";
	}
	void Do(const State::Abandoned& tag)
	{
		std::cout << "Abandoned...\n";
	}
	void Do(const State::IOCompletion& tag)
	{
		std::cout << "IOCompletion...\n";
	}

	template <typename TOverload, typename TVariant, size_t N = 0>
	constexpr bool Check() noexcept
	{
		if constexpr (std::is_invocable_v<TOverload, std::variant_alternative_t<N, TVariant>>)
			return true;
		else if constexpr (N + 1 < std::variant_size_v<TVariant>)
			RuntimeSet<TOverload, TVariant, N + 1>();
		else 
			return false;
	}

	template<typename O, typename...T>
	struct V1
	{
		V1(const O& t) : m_t(t) 
		{
		}

		std::variant<T...> m_v;
		O m_t;
	};

	template<typename T> 
	struct is_variant : std::false_type {};

	template<typename ...Args>
	struct is_variant<std::variant<Args...>> : std::true_type {};

	template<typename T>
	inline constexpr bool is_variant_v = is_variant<T>::value;

	template<typename T>
	concept IsVariant = is_variant_v<T>;

	template<typename T> 
	struct is_overloaded : std::false_type {};

	template<typename ...Args>
	struct is_overloaded<Helper::Overload<Args...>> : std::true_type {};

	template<typename T>
	inline constexpr bool is_overloaded_v = is_overloaded<T>::value;

	template<typename T>
	concept IsOverloaded = is_overloaded_v<T>;

	template<IsVariant TVariant, IsOverloaded TOverload>
	struct VariantOverload
	{
		VariantOverload(const TVariant& variant, const TOverload& overload) 
			: m_overload(overload)
		{
			static_assert(
				Check<TOverload, TVariant>(), 
				"TOverload type must be invocable with all elements types of TVariant"
			);
		}
		VariantOverload(TVariant&& variant, TOverload&& overload)
			: m_overload(std::move(overload))
		{ 
			static_assert(
				Check<TOverload, TVariant>(), 
				"TOverload type must be invocable with all element types of TVariant"
			);
		}

		void Visit()
		{
			std::visit(m_overload, m_variant);
		}

		TVariant m_variant;
		TOverload m_overload;
	};
	struct Tag {};

	template<typename T>
	concept IsVisitable = requires(T t) { t.Visit(); };

	template<bool b>
	[[nodiscard]] auto Get() -> IsVisitable auto
	{
		if constexpr (b)
		{
			return VariantOverload(
				std::variant<Tag>{},
				Helper::Overload{
					[](const Tag& tag) { std::cout << "Tag (A)...\n"; }
				}
			);
		}
		else
		{
			return VariantOverload(
				std::variant<Tag>{},
				Helper::Overload{
					[](const Tag& tag) { std::cout << "Tag (B)...\n"; }
				}
			);
		}
	}

	void Run()
	{
		IsVisitable auto vt = Get<true>();

		HandleUniquePtr eventA(Win32::CreateEventW(nullptr, false, false, nullptr));
		HandleUniquePtr eventB(Win32::CreateEventW(nullptr, false, false, nullptr));
		if (not eventA or not eventB)
			std::terminate();

		std::jthread t(
			[&]{
				std::this_thread::sleep_for(std::chrono::seconds{ 3 });
				Win32::SetEvent(eventB.get());
			});

		WaitState state{};
		std::vector<Win32::HANDLE> handles{ eventA.get(), eventB.get() };
		WaitOn(state, handles, InfiniteWait);

		// https://en.cppreference.com/w/cpp/utility/variant/visit
		// Version 1
		std::visit(
			[](auto&& arg)
			{
				Do(arg);
			},
			state
		);

		// Version 2
		std::visit(
			Helper::Overload{
				[](const State::WaitA& tag)
				{
					std::cout << "WaitA...\n";
				},
				[](const State::WaitB& tag)
				{
					std::cout << "WaitB...\n";
				},
				[](const State::Timeout& tag)
				{
					std::cout << "TagC...\n";
				},
				[](const State::Failed& tag)
				{
					std::cout << "TagC...\n";
				},
				[](const State::Abandoned& tag)
				{
					std::cout << "Abandoned...\n";
				},
				[](const State::IOCompletion& tag)
				{
					std::cout << "IOCompletion...\n";
				}
			},
			state
		);
	}
}
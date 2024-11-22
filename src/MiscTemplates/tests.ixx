export module tests;
export import tuples;
import std;

export namespace FunctionPointers
{
	using SendFnT = void(*)();
	struct SomeSocket
	{
		SendFnT Send;
	};

	struct GH
	{
		~GH()
		{
			std::filesystem::remove("AAAAAA");
		}
	};


	struct HG
	{
		constexpr ~HG()
		{
			if (H)
				delete H;
		}

		auto Get() const
		{
			if (not H) H = new GH();
			return H;
		}

		mutable GH* H = nullptr;
	};

	constexpr HG X; // this should not compile, gcc and clang reject it
}

namespace Looping
{
	template<size_t I, size_t N>
	auto GetValueType(auto&& tuple, std::integral_constant<int, N> val) -> auto
	{
		if constexpr (I == N)
		{
			return std::tuple_element_t<N, std::remove_cvref_t<decltype(tuple)>>{};
		}
		else
		{
			return GetValue<I + 1>(std::forward<decltype(tuple)>(tuple), val);
		};
	}

	template<size_t I, size_t N>
	auto GetValue(auto&& tuple, std::integral_constant<int, N> val) -> std::convertible_to<bool> auto
	{
		if constexpr (I == N)
		{
			return std::tuple_element_t<N, std::remove_cvref_t<decltype(tuple)>>{};
		}
		else
		{
			return GetValue<I + 1>(std::forward<decltype(tuple)>(tuple), val);
		};
	}

	using M = int(*)();

	template<typename T> concept SD = std::convertible_to<T, M>;

	SD auto HM(auto&& X)
	{
		if (X == 1)
		{
			return []() { return 6; };
		}
		else
		{
			throw 1;
		}
	}

	template<typename T>
	struct C
	{
		using TT = T;
	};

	struct D { virtual operator bool() const noexcept = 0; virtual int Do() = 0; };
	struct A : D { virtual operator bool() const noexcept override { return true; } int Do() { return 1; } };
	struct B : D { virtual operator bool() const noexcept override { return true; } int Do() { return 1; } };

	template<typename T>
	concept UI = requires(std::remove_pointer_t<T> t)
	{
		t.Do();
	};

	void DoSomething(int value)
	{
		HM(1);
		std::tuple t{ A{},B{} };
		auto X = GetValue<0>(t, std::integral_constant<int, 1>{});

		constexpr std::variant<A, B> P = B{};

		if (std::holds_alternative<B>(P))
		{

		}

		std::variant<A* (*)(), B* (*)()> Alm = []() { return new B(); };
		UI auto d = std::visit(
			[](auto&& a) -> D*
			{
				static auto* elf = a();
				return a();
			},
			Alm
		);
	}
}

export namespace SizeErased
{
	template<typename T>
	struct SizeErasedArray
	{
		template<size_t S>
		SizeErasedArray(std::array<T, S> array)
			: object(std::make_unique<Model<S>>(std::forward<std::array<T, S>>(array))) {}

		class Iterator
		{
			const SizeErasedArray<T>::Concept* value_ = nullptr;
			int pos = 0;

			public:
			explicit Iterator(const SizeErasedArray<T>::Concept* ptr, int position) : value_{ ptr }, pos(position) {}

			const T& operator*() const
			{
				return value_->Data()[pos];
			}

			Iterator& operator++()
			{
				++pos;
				return *this;
			}

			bool operator!=(const Iterator& other) const
			{
				return pos != other.pos;
			}
		};

		Iterator begin() const { return Iterator{ object.get(), 0 }; }
		Iterator end() const { return Iterator{ object.get(), (int)object->Size() }; }

		private:
		struct Concept
		{
			virtual ~Concept() = default;
			virtual const T* Data() const = 0;
			virtual size_t Size() const noexcept = 0;
		};

		std::unique_ptr<const Concept> object;

		template<size_t S> // (6)
		struct Model final : Concept
		{
			Model(auto&& t) : object(t) {}
			const T* Data() const override
			{
				return object.data();
			}
			size_t Size() const noexcept override
			{
				return object.size();
			}
			private:
			std::array<T, S> object;
		};
	};
}

export namespace SocketsTesting
{
	template<typename T>
	concept Socket = requires(std::remove_cvref_t<T> t)
	{
		t.Send(std::vector<std::byte>{});
	};

	struct SocketA 
	{
		void Send(const std::vector<std::byte>& data) {}
	};
	struct SocketB 
	{
		void Send(const std::vector<std::byte>& data) {}
	};
	static_assert(Socket<SocketA> and Socket<SocketB>);

	template<typename...Ts>
	struct Variant
	{
		template<typename TArg, bool B = std::is_nothrow_invocable_v<TArg>>
		constexpr auto Visit(TArg&& arg) noexcept(B)
		{
			return std::visit(std::forward<TArg>(arg), Sockets);
		}
		std::variant<Ts...> Sockets;
	};

	Variant<SocketA, SocketB> Sockets;
	void SendData(const std::vector<std::byte>& buf)
	{
		Sockets.Visit([&](Socket auto&& s) { s.Send(buf); });
	}

	struct SocketWrapper
	{
		SocketWrapper(int t)
		{
			if (t == 1)
			{
				m_socks.Sockets = SocketB{};
			} // etc
		}

		void Send(const std::vector<std::byte>& data)
		{
			m_socks.Visit([&](Socket auto&& socket) { socket.Send(data); });
		}

		Variant<SocketA, SocketB> m_socks;
	};

	void Run()
	{
	}
}

namespace SomeSetupA
{
	struct A {};

	struct OpA {};
	struct OpB {};

	void N(auto&& lambda)
	{
		auto out = lambda(OpA{});
	}

	void M()
	{
		constexpr auto fn =
			[]<typename TA, typename T>(TA && ta, T && operation) -> auto
			{
				if constexpr (std::same_as<std::remove_cvref_t<T>, OpA>)
					return ta;
				else
					;
			};

		if (1 == 1)
			N([t = A{}](auto&& operation) { return fn(t, std::forward<decltype(operation)>(operation)); });
	}
}

namespace SomeSetupB
{
	struct A { bool Active = false; };
	struct B : A {};
	struct C : A {};

	std::tuple Tuple{ A{false}, B{true}, C{false} };

	A& Get(auto&...types)
	{
		return ((types.IsActive ? (types) : (false)) or ...);
	}

	auto Do() -> A&
	{
		int x = 10;
		auto a = []() {};
		auto b = []() {};

		[i = (1 == 1 ? a : b)]() {}();

		return
			[]<int I = 0>(this auto self) -> A&
		{
			if constexpr (I < std::tuple_size_v<decltype(Tuple)>)
				return std::get<I>(Tuple).Active ? std::get<I>(Tuple) : self.template operator() < I + 1 > ();
			else
				throw std::runtime_error("Exceeded bounds");
		}();
	}
}
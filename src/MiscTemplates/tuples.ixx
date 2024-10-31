export module tuples;
import std;

export namespace TupleAll
{
	template<typename T, typename...Ts>
	concept IsSame = (std::same_as<T, Ts> and ...);

	constexpr auto All =
		[](auto&&... args) constexpr
		{
			if constexpr (sizeof...(args) == 0)
				return true;
			else
				return IsSame<decltype(args)...>;
		};

	void Run()
	{
		constexpr bool b = std::apply(All, std::tuple{ 1, 2 });
		constexpr bool c = std::apply(All, std::tuple{ 1, 2.f });
		std::println("{} {}", b, c);
	}
}

export namespace TupleAll2
{
	constexpr auto All =
		[](auto&& a, auto&&... args) constexpr
		{
			return (std::same_as<decltype(a), decltype(args)> and ...);
		};

	constexpr auto All2 =
		[](auto&&... args) constexpr
		{
			if constexpr (sizeof...(args) == 0 or sizeof...(args) == 1)
				return true;
			else
				return All(args...);
		};


	void Run()
	{
		constexpr bool b = std::apply(All, std::tuple{ 1, 2 });
		constexpr bool c = std::apply(All, std::tuple{ 1, 2.f });
		std::println("{} {}", b, c);
	}
}

export namespace TupleAll3
{
	constexpr bool Something(const std::tuple<>&) { return true; };

	template<typename T, typename...TRest>
	constexpr bool Something(const std::tuple<T, TRest...>&) { return (std::same_as<T, TRest> and ...); };

	void Run()
	{
		std::println(
			"{} {} {}",
			Something(std::tuple{ }),
			Something(std::tuple{ 1, 2, 3 }),
			Something(std::tuple{ 1, 2, 3.f })
		);
	}
}

export namespace TupleAll4
{
	template<typename...T>
	constexpr bool AllSame(const std::tuple<T...>& tuple)
	{
		if constexpr (sizeof...(T) == 0)
			return true;
		else
			return[]<typename THead, typename...TRest>(const std::tuple<THead, TRest...>&) constexpr
			{
				return (std::same_as<THead, TRest> and ...);
			}(tuple);
	}

	void Run()
	{
		AllSame(std::tuple{ });
		AllSame(std::tuple{ 1 });
	}
}

export namespace TypeIndexes
{
	template<typename... TArgs>
	struct Types
	{
		enum { Arity = sizeof...(TArgs) };

		constexpr static int Arity2 = sizeof...(TArgs);

		template<size_t I>
		struct ElementType
		{
			using Type = std::tuple_element_t<I, std::tuple<TArgs...>>;
		};

		static void Invoke(auto&& f)
		{
			//f.operator()<TArgs...>();
		}

		static auto At(int x, auto&& func)
		{
			int iter = 0;
			return ([x, &func]<typename T = TArgs>(int iter)
			{
				return x == iter ? func(T{}), true : false;
			}(iter++) or ...);

			return ([]<size_t...Is>(int x, std::index_sequence<Is...>)
			{
				return ([]<typename T = TArgs>(int x)
				{
					if (x == Is)
					{
						std::println("Found it!");
						return true;
					}
					return false;
				}(x) or ...);
			}(x, std::make_index_sequence<Arity>{}));
		}

		static void Each()
		{
			([]<typename T = TArgs>()
			{
				if constexpr (std::constructible_from<T, int>)
					std::println("Type is constructible with int.");
				else
					std::println("Type is default constructible.");
			}(), ...);
		}

		void ForEach()
		{
			Types<TArgs...>::Each();
		}
	};

	struct A
	{
		//A(int) {}
	};

	struct B
	{
		void operator ()() {}
	};

	struct C
	{
		void operator ()() {}
	};

	template<typename...T>
	void AnotherOne() {}

	void Run()
	{
		using F = Types<A, B, C>;
		F::At(2, [](auto&& a) {std::println("OK!"); });

		F::ElementType<0>::Type o;
		F f{};
		f.Each();
		f.Invoke([]<typename...T>() { std::println("something something"); });
		f.Invoke(AnotherOne<int>);

		std::make_index_sequence<F::Arity> a{};
	}

	struct D
	{
		virtual ~D() = 0 {};
		virtual void DoIt() = 0;
	};

	struct E : public D
	{
		void DoIt() override {}
	};

	static_assert(std::derived_from<E, D>);
}


export namespace Splitter
{
	template<size_t VSize>
	std::vector<std::byte> CreateSequencedVector()
	{
		return []<size_t...Is>(std::index_sequence<Is...>)
		{
			return std::vector<std::byte> { static_cast<std::byte>(Is % 255)... };
		}(std::make_index_sequence<VSize>{});
	}

	std::vector<std::vector<std::byte>> Split(std::vector<std::byte>& toSplit)
	{
		std::vector<std::vector<std::byte>> returnValue;

		constexpr auto splitAmount = 256;
		auto maxSize = toSplit.size();
		auto ptr = toSplit.begin();
		size_t totalRead = 0;


		/*auto y = toSplit | std::ranges::views::split(300);

		auto x = toSplit | std::ranges::views::take(3000);
		std::ranges::for_each(x, [](std::byte t) { std::println("Took {}", static_cast<unsigned short>(t)); });*/

		while (true)
		{
			auto remaining = maxSize - totalRead;
			auto toRead = remaining > splitAmount ? splitAmount: remaining;
			

			std::vector<std::byte> v{ ptr, ptr + toRead };
			returnValue.push_back(std::move(v));

			totalRead += toRead;
			ptr += toRead;

			if (totalRead >= maxSize)
				break;
		}

		return returnValue;
	}

	enum class SplitError
	{
		NoRemainder
	};

	std::expected<std::vector<std::byte>, SplitError> Split(std::vector<std::byte>& toSplit, int iter, size_t splitAmount)
	{
		auto maxSize = toSplit.size();
		auto ptr = toSplit.begin();
		size_t totalRead = splitAmount * iter;

		if (totalRead >= maxSize)
			return std::unexpected(SplitError::NoRemainder);

		auto remaining = maxSize - totalRead;
		auto toRead = remaining > splitAmount ? splitAmount : remaining;

		return std::vector<std::byte>{ ptr, ptr + toRead };
	}

	void ReadFromFile()
	{
		std::basic_ifstream<std::byte> file("test.txt", std::ios_base::in | std::ios_base::binary);
		if (file.fail())
			return;

		std::vector<std::byte> read;
		std::byte readBuffer[3];
		while (not file.eof())
		{
			file.read(readBuffer, 3);
			read.insert(read.end(), std::begin(readBuffer), std::begin(readBuffer)+file.gcount());
		}
		
		std::string s(reinterpret_cast<char*>(read.data()), read.size());
		std::println("Read a total of {} bytes: {}", read.size(), s);
	}

	template<bool VBinary>
	struct FileReader
	{
		template<bool VIsBinary> struct StreamTypeTraits;
		template<> struct StreamTypeTraits<false>
		{
			using BasicType = char;
			using BufferType = std::vector<char>;
			using Type = std::ifstream;
			static constexpr bool Flags = std::ios::in;
		};
		template<> struct StreamTypeTraits<true>
		{
			using BasicType = std::byte;
			using BufferType = std::vector<std::byte>;
			using Type = std::basic_ifstream<std::byte>;
			static constexpr bool Flags = std::ios::in | std::ios_base::binary;
		};
		using StreamTraits = StreamTypeTraits<VBinary>;
		using StreamType = StreamTraits::Type;
		using BufferType = StreamTraits::BufferType;
		using BasicType = StreamTraits::BasicType;

		FileReader(const std::filesystem::path p)
			: Path(std::move(p))
		{
			if (not std::filesystem::exists(Path))
				throw std::runtime_error("Non-existent file.");
			FileStream = StreamType(p.string(), StreamTypeTraits::Flags);
			if (FileStream.fail())
				throw std::runtime_error("Stream failed to open file.");
		}

		size_t Size() const
		{
			return std::filesystem::file_size(Path);
		}

		enum class ReadError {EOF};

		std::expected<BufferType, ReadError> Read(size_t amount)
		{
			if (FileStream.eof())
				return std::unexpected(ReadError::EOF);

			BufferType returnValue(amount, BasicType{});
			FileStream.read(amount, returnValue.data());
			returnValue.resize(FileStream.gcount());
			return returnValue;
		}

		auto begin() const
		{
			return FileStream.begin();
		}

		auto end() const
		{
			return FileStream.end();
		}

		private:
		std::filesystem::path Path;
		StreamType FileStream;
	};

	using TextFileStream = FileReader<false>;
	using BinFileStream = FileReader<true>;

	void Chunked()
	{
		std::vector v{ 1,3,4,5 };
		auto x = v | std::views::chunk(2);
	}

	void Run()
	{
		ReadFromFile();

		std::vector data = CreateSequencedVector<1024>();
		std::vector split = Split(data);
		std::println("{} vectors, last {}", split.size(), split.back().size());

		for (int i = 0; auto x = Split(data, i, 256); i++)
		{
			std::println("Iter {}, got vector of size {}", i, x->size());
		}

		//std::ranges::for_each(data, [](std::byte b) { std::println("{}", static_cast<unsigned short>(b)); });
	}
}
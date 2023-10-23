export module stringtemplates;
import std;

export namespace FixedStrings1
{
	// See https://stackoverflow.com/questions/2033110/passing-a-string-literal-as-a-type-argument-to-a-class-template
	template<unsigned N>
	struct FixedString
	{
		char buf[N + 1]{};
		constexpr FixedString(char const* s)
		{
			for (unsigned i = 0; i < N; i++)
				buf[i] = s[i];
		}
		constexpr operator char const* () const
		{
			return buf;
		}
	};
	template<unsigned N>
	FixedString(char const (&)[N]) -> FixedString<N - 1>;

	template<FixedString T>
	class Foo
	{
		static constexpr char const* Name = T;
	public:
		void hello() const
		{
			std::cout << Name << std::endl;
		};
	};

	template <size_t N>
	struct FixedString2
	{
		char buf[N]{};
		constexpr FixedString2(const char(&arg)[N])
		{
			for (unsigned i = 0; i < N; i++)
				buf[i] = arg[i];
		}

		constexpr operator char const* () const
		{
			return buf;
		}

		static void Blah()
		{
			std::cout << buf << std::endl;
		}
	};
	template<size_t N>
	FixedString2(char const (&)[N]) -> FixedString2<N>;

	template<FixedString2 T>
	class Foo2
	{
	public:
		static constexpr const char* Name = T;
		void hello() const
		{
			std::cout << Name << std::endl;
		};
	};


	enum class SomeEnum
	{
		Entry1 = 'A',
		Entry2,
		Entry3
	};

	template<SomeEnum S>
	struct Command
	{
		void operator()()
		{
			std::cout << 1 << std::endl;
		}
	};

	template<>
	struct Command<SomeEnum::Entry2>
	{
		Command(int x) {}

		void operator()()
		{
			std::cout << 2 << std::endl;
		}
	};

	template<>
	struct Command<SomeEnum::Entry3>
	{
		static void Execute()
		{
			std::cout << 2 << std::endl;
		}
	};

	void Run()
	{
		Foo<"Hello!"> foo;
		foo.hello();

		FixedString s = "AA";
		FixedString2 s2 = "AA";
		char b[] = "Am";
		FixedString c = b;

		Foo2<"P"> p;
		Foo2<"O"> o;
		p.hello();
		o.hello();
		//H a = "A";
		//H b = L"A";
		//H c = L"A";

		Command<SomeEnum::Entry1>()();
		Command<SomeEnum::Entry2>(1)();
		Command<SomeEnum::Entry3>::Execute();
	}
}

export namespace FixedStrings2
{
	// See https://dev.to/sgf4/strings-as-template-parameters-c20-4joh
	template<std::size_t N>
	struct CompTimeStr {
		char data[N]{};

		consteval CompTimeStr(const char(&str)[N]) {
			std::copy_n(str, N, data);
		}

		consteval bool operator==(const CompTimeStr<N> str) const {
			return std::equal(str.data, str.data + N, data);
		}

		template<std::size_t N2>
		consteval bool operator==(const CompTimeStr<N2> s) const {
			return false;
		}

		template<std::size_t N2>
		consteval CompTimeStr<N + N2 - 1> operator+(const CompTimeStr<N2> str) const {
			char newchar[N + N2 - 1]{};
			std::copy_n(data, N - 1, newchar);
			std::copy_n(str.data, N2, newchar + N - 1);
			return newchar;
		}

		consteval char operator[](std::size_t n) const {
			return data[n];
		}

		consteval std::size_t size() const {
			return N - 1;
		}
	};

	template<std::size_t s1, std::size_t s2>
	consteval auto operator+(CompTimeStr<s1> fs, const char(&str)[s2]) {
		return fs + CompTimeStr<s2>(str);
	}

	template<std::size_t s1, std::size_t s2>
	consteval auto operator+(const char(&str)[s2], CompTimeStr<s1> fs) {
		return CompTimeStr<s2>(str) + fs;
	}

	template<std::size_t s1, std::size_t s2>
	consteval auto operator==(CompTimeStr<s1> fs, const char(&str)[s2]) {
		return fs == CompTimeStr<s2>(str);
	}

	template<std::size_t s1, std::size_t s2>
	consteval auto operator==(const char(&str)[s2], CompTimeStr<s1> fs) {
		return CompTimeStr<s2>(str) == fs;
	}

	template<CompTimeStr str>
	constexpr auto addBar() {
		return str + " bar";
	}

	void Run() 
	{
		constexpr CompTimeStr str = addBar<"foo">();
		std::cout << str.data << std::endl;
		std::cout << std::boolalpha << (str == "foo bar") << std::endl;
	}
}

export namespace FixedString3
{
	template <size_t N>
	struct FixedString
	{
		wchar_t buf[N]{};
		consteval FixedString(const wchar_t(&arg)[N]) noexcept
		{
			std::copy_n(arg, N, buf);
		}

		consteval operator const wchar_t* () const noexcept
		{
			return buf;
		}

		template<std::size_t N2>
		consteval FixedString<N + N2 - 1> operator+(const FixedString<N2> str) const 
		{
			char newchar[N + N2 - 1]{};
			std::copy_n(buf, N - 1, newchar);
			std::copy_n(str.buf, N2, newchar + N - 1);
			return newchar;
		}
	};
	template<size_t N>
	FixedString(wchar_t const (&)[N]) -> FixedString<N>;
}

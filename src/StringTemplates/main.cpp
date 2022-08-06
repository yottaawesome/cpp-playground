#include <iostream>
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
FixedString(char const (&)[N])->FixedString<N - 1>;

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
FixedString2(char const (&)[N])->FixedString2<N>;

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
	Entry1,
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

int main(int argc, char* argv[])
{
	Foo<"Hello!"> foo;
	foo.hello();

	FixedString s = "AA";
	FixedString2 s2 = "AA";

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

    return 0;
}

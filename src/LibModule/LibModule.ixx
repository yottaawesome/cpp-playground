module; // everything below is part of the implicit global module fragment

#include <iostream>

export module LibModule;

namespace TestNamespace
{
	void InternalPrint();
	export 
	{
		void MyFunc()
		{
			std::wcout << L"Hello World from TestNamespace::MyFunc()!" << std::endl;
		}

		class Test
		{
			public:
				Test() {}
				void Print()
				{
					InternalPrint();
				}
		};

		class Test2
		{
			public:
				Test2();
				void Print();
		};

		struct ITest3
		{
			virtual void Func() = 0;
			virtual ~ITest3(); // = 0 (abstract not allowed here);
		};
	}
}

module :private;

namespace TestNamespace
{
	void InternalPrint()
	{
		std::wcout << L"Hello World from TestNamespace::Test!" << std::endl;
	}
}

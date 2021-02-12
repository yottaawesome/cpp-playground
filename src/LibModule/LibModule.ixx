export module LibModule;

export namespace TestNamespace
{
	void MyFunc();

	class Test
	{
		public:
			Test();
			void Print();
	};

	class Test2
	{
		public:
			Test2();
			void Print();
	};
}

module :private;

namespace TestNamespace
{
	void InternalPrint();
}

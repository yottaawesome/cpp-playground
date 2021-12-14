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

	struct ITest3
	{
		virtual void Func() = 0;
		virtual ~ITest3(); // = 0 (abstract not allowed here);
	};
}

module :private;

namespace TestNamespace
{
	void InternalPrint();
}

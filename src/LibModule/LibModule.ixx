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
}

module :private;

namespace TestNamespace
{
	void InternalPrint();
}

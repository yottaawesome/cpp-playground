export module DllModule;
import std;

// may need to put __declspec before the return type to appease the compiler sometimes
// Functions need the __declspec(dllexport).
export int __declspec(dllexport) fnDllModule();

export extern "C" int __declspec(dllexport) returnNumber();

export namespace Another
{
	__declspec(dllexport) void Sup() {};
}

export namespace ERT
{
	export class __declspec(dllexport) XX
	{
		public:
			void MM();
	};
}

// Classes don't need __declspec(dllexport)
export class AA
{
	public:
		void MM() {};
		void NN() {};
		void OO()
		{
			throw std::runtime_error("This is a test");
		}
		static void YY() {};
};

export struct __declspec(dllexport) F
{
	virtual ~F() = default;
	virtual void Blah() = 0;
};

export class __declspec(dllexport) DllClass
{
	public:
		void Hello();
};
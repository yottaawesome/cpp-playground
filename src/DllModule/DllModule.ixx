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
	export struct __declspec(dllexport) XX
	{
		void MM();
	};
}

// Classes don't need __declspec(dllexport) if
// functions are defined in the declaration.
// 14/06/2025 -- now needed.
export struct __declspec(dllexport) AA
{
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

// __declspec(dllexport) is required when definitions
// are in implementation files.
export struct __declspec(dllexport) DllClass
{
	void Hello();
};

// C4275
// Can be ignored https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170
#pragma warning(disable: 4275)
export struct __declspec(dllexport) Error : std::runtime_error 
{
	Error() : std::runtime_error("") {}
};

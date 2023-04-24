export module DllModule;

// may need to put __declspec before the return type to appease the compiler sometimes
export int __declspec(dllexport) fnDllModule();

export extern "C" int __declspec(dllexport) returnNumber();

export class AA
{
	public:
		void MM() {};
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
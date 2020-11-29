export module DllModule;

export int __declspec(dllexport) fnDllModule();

export extern "C" int __declspec(dllexport) fnDllModule2();

export class __declspec(dllexport) DllClass
{
	public:
		void Hello();
};
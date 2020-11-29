export module DllModule;

export int __declspec(dllexport) fnDllModule();

export class __declspec(dllexport) DllClass
{
	public:
		void Hello();
};
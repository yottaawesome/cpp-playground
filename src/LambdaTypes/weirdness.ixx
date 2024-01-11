export module weirdness;
import std;

export namespace Weirdness
{
	void Run()
	{
		const int n = 1;
		int l = 
			[n=n]() mutable
			{
				return ++n;
			}();
		std::println("{}-{}", n, l);
	}
}
export module module1;
import std;
import std.compat;
import module2;

// This demonstrates how to use the standard module in 
// the presence of old header-based components. We use 
// other modules to export the header contents, which 
// allows us to use the standard module.
export namespace Other
{
	void SomeFunction()
	{
		uint64_t SomeNumber = 1;
		std::string m;
		Test2::SomeClass s;
		s.Number = 10;
		std::cout << std::format("{} {}\n", SomeNumber, s.Number);
		Y y;
	}
}
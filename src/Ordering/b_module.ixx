export module b_module;
import std;
import a_module;

export
{
	// Results in both b and c not printing
	std::string b = a.c_str(); 
	// Works if first line in commented
	std::string c = []() { return a.c_str(); }();
	std::string d = AA;
}
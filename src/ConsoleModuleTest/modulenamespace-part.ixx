export module modulenamespace:part;

import std;

namespace Blah
{
	using F = int;
}

namespace Blah2
{
	using A = int;
}

// These are not exported and so are not visible
// in consuming code. This is unlike headers.
using namespace Blah;
using namespace Blah2;
using namespace std;
using namespace std::chrono;

export namespace NamespaceTest
{
	F Get() { return 1; }

	string GetAString()
	{
		return "AAAA!";
	}

	milliseconds GetMillis()
	{
		return milliseconds{ 0 };
	}
}

namespace DontExport
{
	struct AType {};
}

namespace DontExport2
{
	using namespace DontExport;
	AType ThisIsAnotherBlah() 
	{
		return {};
	}
}

namespace DontExport3
{
	using namespace DontExport;
}

export
{
	using ::DontExport2::ThisIsAnotherBlah;
}
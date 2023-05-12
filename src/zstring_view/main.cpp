#include <iostream>
#include <string>

import zstring_view;

int main()
{
    // A null-terminated string-view. This is useful for dealing
    // with C APIs that expect a null-terminated string. The
    // regular std::string_view allows the possibility of being
    //
    View::zstring_view blah{ "Hello, world!\n" };
    std::cout << blah;
    std::string

    int m = View::x;

    return 0;
}

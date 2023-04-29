#include <iostream>
#include <format>
#include <stacktrace>

void AnotherFunction()
{
    const std::stacktrace st = std::stacktrace::current();   
    for (const auto& ste : st)
    {
        std::cout << std::format(
            "Entry:\n\tDescription: {}\n\tSource file: {}\n\tSource line: {}\n", 
            ste.description(),
            ste.source_file(),
            ste.source_line()
        );
        // Break on this to avoid logging VC runtime functions
        if (ste.description().starts_with("StackTrace!main"))
            break;
    }
}

void AFunction()
{
    AnotherFunction();
}

int main()
{
    AFunction();
    return 0;
}

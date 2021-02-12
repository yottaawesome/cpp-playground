#include <iostream>
#include <filesystem>

int main(int argc, char* args[])
{
    std::filesystem::path currentPath = std::filesystem::current_path();
    currentPath = currentPath.parent_path();

    std::wcout << currentPath.c_str() << std::endl;

    return 0;
}

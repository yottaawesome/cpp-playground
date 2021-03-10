#include <iostream>
#include <filesystem>

int main(int argc, char* args[])
{
    std::filesystem::path currentPath = std::filesystem::current_path();
    currentPath = currentPath.parent_path();
    std::wcout << currentPath.c_str() << std::endl;

    currentPath = std::filesystem::current_path();
    currentPath = "..";
    //std::filesystem::current_path();
    //currentPath += "\\..";
    std::wcout << std::filesystem::absolute(currentPath).c_str() << std::endl;

    std::filesystem::path testPath = "C:\\Temp\\Temp2";
    std::filesystem::create_directories(testPath);

    return 0;
}

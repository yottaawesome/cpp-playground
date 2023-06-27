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

    std::filesystem::path testPath2 = "C:/Temp/Temp2/Temp3";
    for (auto& x : testPath2)
    {
        std::cout << x << " ";
    }
    std::filesystem::path testPath3 = "Temp\\Temp2\\Temp3";
    for (auto& x : testPath3)
    {
        std::cout << x << " ";
    }

    return 0;
}

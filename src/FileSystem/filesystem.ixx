export module filesystem;
import std;

export namespace Filesystem::BasicTest
{
    void Test()
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
    }
}

export namespace Filesystem::Directory
{
    void Run()
    {
        // See https://stackoverflow.com/a/32889434/7448661
        std::filesystem::path path("TestDir");
        for (const std::filesystem::directory_entry& dirEntry : std::filesystem::recursive_directory_iterator(path))
        {
            std::println("{} -> {}", dirEntry.is_directory() ? "directory" : "file", dirEntry.path().string());
        }
    }
}
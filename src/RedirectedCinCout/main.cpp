#include <iostream>
#include <fstream>
#include <string>

void f()
{
    std::string line;
    while (std::getline(std::cin, line))  //input from the file in.txt
    {
        std::cout << line << "\n";   //output to the file out.txt
    }
}

int main(int argc, char* argv[])
{
    // Adapted from https://stackoverflow.com/questions/10150468/how-to-redirect-cin-and-cout-to-files
    // Specifically https://stackoverflow.com/a/10151286/7448661
    std::ifstream in("in.txt");
    std::streambuf* cinbuf = std::cin.rdbuf(); //save old buf
    std::cin.rdbuf(in.rdbuf()); //redirect std::cin to in.txt!

    std::ofstream out("out.txt");
    std::streambuf* coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

    std::string word;
    std::cin >> word;           //input from the file in.txt
    std::cout << word << "  ";  //output to the file out.txt

    f(); //call function

    std::cin.rdbuf(cinbuf);   //reset to standard input again
    std::cout.rdbuf(coutbuf); //reset to standard output again

    std::cin >> word;   //input from the standard input
    std::cout << word;  //output to the standard input

    return 0;
}

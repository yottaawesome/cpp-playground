//import somemodule;

int main(int argc, char* argv[])
{
	//Blah::Oops();
	return 0;
}

// clang++ -std=c++20 -c --target=x86_64-pc-windows-msvc19.38.33130 somemodule.cppm --precompile -o somemodule.pcm
// clang++ -std=c++20 main.cpp --target=x86_64-pc-windows-msvc19.38.33130 -fprebuilt-module-path=. somemodule.pcm -o client.exe

// clang++ -std=c++20 -c somemodule.cppm --precompile -o somemodule.pcm
// clang++ -std=c++20 main.cpp -fprebuilt-module-path=. somemodule.pcm -o helloclangwindows.exe
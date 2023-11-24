# Compiling C++ with Visual Studio and Linux

## Introduction

* You need a WSL2 distro.
* Install required tools on the VM: `sudo apt install -y openssh-server build-essential gdb rsync make zip`
* [Enable SSH on the VM](https://jmmv.dev/2022/02/wsl-ssh-access.html). Test locally with `ssh -p 22 <username>@localhost`.
* Configure VS to connect to the VM: `TOOLS > Options > Cross Platform > Connection Manager`.
* [Install additional GCC/g++ versions if necessary](https://phoenixnap.com/kb/install-gcc-ubuntu). Update the C++ Compiler option to point to the preferred compiler (and don't forget to change the language standard and enable additional flags like `-fmodules-ts`).

## Additional resources

* [C++20: Module Support of the Big Three](https://www.modernescpp.com/index.php/c20-module-support-of-the-big-three-compilers/)
* [C++20: More Details about Module Support of the Big Three](https://www.modernescpp.com/index.php/c20-more-details-about-module-support-of-the-big-three/)
* [Walkthrough: Build and debug C++ with WSL 2 and Visual Studio 2022](https://learn.microsoft.com/en-us/cpp/build/walkthrough-build-debug-wsl2?view=msvc-170)
* [Tutorial: Create C++ cross-platform projects in Visual Studio](https://learn.microsoft.com/en-us/cpp/build/get-started-linux-cmake?view=msvc-170)
* [C++ Compiler Support](https://en.cppreference.com/w/cpp/compiler_support): useful to ensure features used in your cross-platform code align with your available compilers.

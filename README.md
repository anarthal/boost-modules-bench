# Boost modules benchmark and POC

This repository contains:

- A proof-of-concept on how to export Asio names as a C++20 module.
- A CMake project that installs the Asio module, providing a CMake function to build the Asio module.
- A CMake project that consumes the installed Asio module, builds it and uses it to build a server. It also uses `import std`. The server was used to benchmark compile times.

Important files:

- [Asio module code](asio-module/module/asio.cxx)
- Asio module CMake: [project](asio-module/CMakeLists.txt) and [installed CMake](asio-module/AsioConfig.cmake.in).
- [Server CMake](CMakeLists.txt).
- [Server code](main.hpp).

Note that Asio and stdlib have been copied (for simplicity) from the standalone asio repo and the libc++ Ubuntu distribution.

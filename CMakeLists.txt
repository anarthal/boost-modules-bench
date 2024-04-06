cmake_minimum_required(VERSION 3.28)
project(modules_benchmark CXX)

find_package(OpenSSL REQUIRED)

# Use modules?
set(USE_MODULES ON)

# Always use libc++
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-stdlib=libc++ -ftime-trace)
add_link_options(-stdlib=libc++)

# Disable module scanning
if (USE_MODULES)
    set(CMAKE_CXX_SCAN_FOR_MODULES ON)
else()
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

# Build the stdlib module
function (add_stdlib_module NAME)
    add_library(${NAME})
    target_sources(${NAME} PUBLIC 
        FILE_SET CXX_MODULES FILES
        stdlib/std.cppm
        stdlib/std.compat.cppm
    )
    target_compile_features(${NAME} PUBLIC cxx_std_23)
    target_compile_options(${NAME} PRIVATE -Wno-reserved-module-identifier)
endfunction()

if (USE_MODULES)
    # Find the Asio package containing the function to build the module
    find_package(Asio REQUIRED)

    # Build the stdlib module
    add_stdlib_module(stdlib)

    # Build the asio module
    add_asio_module(asio)
else()
    # Asio interface library
    add_library(asio INTERFACE)
    target_include_directories(asio INTERFACE asio-module/include)
    target_compile_features(asio INTERFACE cxx_std_23)
    target_link_libraries(asio INTERFACE OpenSSL::SSL OpenSSL::Crypto)
endif()

# Executable
add_executable(server main.cpp
    main2.cpp
    main3.cpp
    main4.cpp
    main5.cpp
    main6.cpp
    main7.cpp
)
target_compile_features(server PRIVATE cxx_std_23)
if (USE_MODULES)
    target_link_libraries(server PRIVATE stdlib asio)
    target_compile_definitions(server PRIVATE USE_MODULES)
    target_compile_options(server PRIVATE -Wno-deprecated-declarations)
else()
    target_link_libraries(server PRIVATE asio)
endif()

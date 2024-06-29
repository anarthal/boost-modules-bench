cmake_minimum_required(VERSION 3.28...3.30)
project(modules_benchmark LANGUAGES CXX)

find_package(OpenSSL REQUIRED)

# Use modules?
option(USE_MODULES "on/off" ON)

# Always use libc++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-stdlib=libc++ -ftime-trace)
add_link_options(-stdlib=libc++)

# Disable module scanning
if(USE_MODULES)
    set(CMAKE_CXX_SCAN_FOR_MODULES ON)
else()
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if(PROJECT_IS_TOP_LEVEL)
  include(CheckCXXSymbolExists)
  check_cxx_symbol_exists(snprintf cstdio ASIO_HAS_SNPRINTF)
  check_cxx_symbol_exists(sprintf_s cstdio ASIO_HAS_SECURE_RTL)
endif()

set(CPPdefinitions ASIO_NO_DEPRECATED)
if(ASIO_HAS_SECURE_RTL)
  list(APPEND CPPdefinitions ASIO_HAS_SECURE_RTL)
elseif(ASIO_HAS_SNPRINTF)
  list(APPEND CPPdefinitions ASIO_HAS_SNPRINTF)
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
    target_compile_definitions(${NAME} PUBLIC _LIBCPP_HAS_NO_LOCALIZATION)
    target_compile_options(${NAME} PRIVATE -Wno-reserved-module-identifier)
endfunction()

if(USE_MODULES)
    # Find the Asio package containing the function to build the module
    find_package(Asio REQUIRED HINTS stagedir/lib)

    # Build the stdlib module
    if(NOT APPLE)
        add_stdlib_module(stdlib)
    endif()

    # Build the asio module
    add_asio_module(asio)
else()
    # Asio interface library
    add_library(asio INTERFACE)
    target_compile_definitions(asio INTERFACE ${CPPdefinitions})
    target_include_directories(asio INTERFACE asio-module/include)
    target_compile_features(asio INTERFACE cxx_std_23)
    target_link_libraries(asio INTERFACE OpenSSL::SSL OpenSSL::Crypto)
endif()

# Executable
add_executable(server main.cpp main.hpp
    main2.cpp
    main3.cpp
    main4.cpp
    main5.cpp
    main6.cpp
    main7.cpp
)
target_compile_features(server PRIVATE cxx_std_23)
if(USE_MODULES)
    if(APPLE)
        target_link_libraries(server PRIVATE asio)
    else()
        target_link_libraries(server PRIVATE stdlib asio)
    endif()
    target_compile_definitions(server PRIVATE USE_MODULES)
    #XXX target_compile_options(server PRIVATE -Wno-deprecated-declarations)
else()
    target_link_libraries(server PRIVATE asio)
endif()

enable_testing()
if(APPLE)
    add_test(NAME server COMMAND dyld_info server)
else()
    add_test(NAME server COMMAND ldd server)
endif()

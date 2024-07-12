cmake_minimum_required(VERSION 3.28.4...3.30)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD ON)

project(modules_benchmark LANGUAGES CXX)

find_package(OpenSSL REQUIRED)

# Use modules?
unset(USE_MODULES)
unset(HAS_STDLIB_MODULES)

# Always use libc++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.28.4 AND CMAKE_GENERATOR STREQUAL "Ninja")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 17.0)
        set(USE_MODULES TRUE)
        # see https://releases.llvm.org/15.0.0/projects/libcxx/docs/ReleaseNotes.html
        add_compile_options(-fexperimental-library)
        add_link_options(-lc++experimental)
        add_compile_options(-ftime-trace)
        add_compile_options(-stdlib=libc++)
        add_link_options(-stdlib=libc++)
        if(DEFINED ENV{LLVM_ROOT})
            message(STATUS "LLVM_ROOT=$ENV{LLVM_ROOT}")
            set(LLVM_LIBC_SOURCE $ENV{LLVM_ROOT}/share/libc++/v1)
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 18.0)
                set(HAS_STDLIB_MODULES stdlib)
                set(CMAKE_CXX_MODULE_STD ON)
                set(CMAKE_CXX_COMPILER_IMPORT_STD ON)
            endif()
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14.0)
        set(USE_MODULES TRUE)
        if(DEFINED ENV{GNU_ROOT})
            message(STATUS "GNU_ROOT=$ENV{GNU_ROOT}")
            message(STATUS "CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES=${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}")
            # FIXME: include_directories(BEFORE SYSTEM ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
            if(APPLE AND DEFINED ENV{LLVM_ROOT})
                set(LLVM_LIBC_SOURCE $ENV{LLVM_ROOT}/share/libc++/v1)
                set(HAS_STDLIB_MODULES stdlib)
                # XXX add_compile_options(-isystem $ENV{GNU_ROOT}/include/c++/14/x86_64-apple-darwin23)
                # XXX add_compile_options(-I $ENV{GNU_ROOT}/include/c++/14)
            endif()
        endif()
    endif()
endif()

# Disable module scanning
if(USE_MODULES)
    set(CMAKE_CXX_SCAN_FOR_MODULES ON)
    if(CMAKE_CXX_COMPILER_IMPORT_STD)
        message(STATUS "CMAKE_CXX_COMPILER_IMPORT_STD=${CMAKE_CXX_COMPILER_IMPORT_STD}")
    endif()
else()
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if(NOT EXISTS ${LLVM_LIBC_SOURCE})
    set(LLVM_LIBC_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/stdlib)
endif()

if(USE_MODULES)
    message(STATUS "USE_MODULES=${USE_MODULES}")
    message(STATUS "LLVM_LIBC_SOURCE=${LLVM_LIBC_SOURCE}")
endif()

if(PROJECT_IS_TOP_LEVEL)
    include(CheckCXXSymbolExists)
    check_cxx_symbol_exists(snprintf cstdio ASIO_HAS_SNPRINTF)
    check_cxx_symbol_exists(sprintf_s cstdio ASIO_HAS_SECURE_RTL)
endif()

list(APPEND CPPdefinitions ASIO_NO_DEPRECATED ASIO_STANDALONE)
if(ASIO_HAS_SECURE_RTL)
    list(APPEND CPPdefinitions ASIO_HAS_SECURE_RTL)
elseif(ASIO_HAS_SNPRINTF)
    list(APPEND CPPdefinitions ASIO_HAS_SNPRINTF)
endif()

# Build the stdlib module
function(add_stdlib_module NAME)
    add_library(${NAME})
    add_library(__CMAKE::CXX23 ALIAS ${NAME})
    # cmake-format: off
    target_sources(${NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${LLVM_LIBC_SOURCE}
        FILES
            ${LLVM_LIBC_SOURCE}/std.cppm
            ${LLVM_LIBC_SOURCE}/std.compat.cppm
    )
    # cmake-format: on
    target_compile_features(${NAME} PUBLIC cxx_std_23)
    target_compile_definitions(${NAME} PUBLIC _LIBCPP_HAS_NO_LOCALIZATION)
    target_compile_options(${NAME} PRIVATE -Wno-reserved-module-identifier)
endfunction()

if(USE_MODULES)
    # Find the Asio package containing the function to build the module
    find_package(Asio REQUIRED HINTS stagedir/lib)

    # Build the stdlib module
    if(HAS_STDLIB_MODULES)
        message(STATUS "HAS_STDLIB_MODULES=${HAS_STDLIB_MODULES}")
        add_stdlib_module(${HAS_STDLIB_MODULES})
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
add_executable(
    server
    main.cpp
    main.hpp
    main2.cpp
    main3.cpp
    main4.cpp
    main5.cpp
    main6.cpp
    main7.cpp
)
target_compile_features(server PRIVATE cxx_std_23)
target_compile_options(server PRIVATE -Wno-deprecated-declarations)
if(USE_MODULES)
    target_link_libraries(server PRIVATE ${HAS_STDLIB_MODULES} asio)
    target_compile_definitions(server PRIVATE USE_MODULES)
    if(HAS_STDLIB_MODULES)
        target_compile_definitions(server PRIVATE HAS_STDLIB_MODULES)
    endif()
else()
    target_link_libraries(server PRIVATE asio)
endif()

enable_testing()
if(APPLE)
    add_test(NAME server COMMAND dyld_info server)
else()
    add_test(NAME server COMMAND ldd server)
endif()

add_custom_target(
    format-fix COMMAND clang-format -style=file -i $<TARGET_PROPERTY:asio,CXX_MODULE_SET> COMMAND_EXPAND_LISTS
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

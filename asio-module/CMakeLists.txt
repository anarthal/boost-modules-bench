cmake_minimum_required(VERSION 3.28.0)
project(asio_module)

install(DIRECTORY include DESTINATION .)
install(DIRECTORY module DESTINATION .)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    AsioConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

configure_file(AsioConfig.cmake.in AsioConfig.cmake @ONLY)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/AsioConfig.cmake
    DESTINATION lib/cmake/Asio
)
cmake_minimum_required(VERSION 3.28.4...3.31)
project(asio_module LANGUAGES CXX)
set(CMAKE_INSTALL_MESSAGE NEVER)

install(DIRECTORY include DESTINATION .)
install(DIRECTORY module DESTINATION .)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(AsioConfigVersion.cmake VERSION 1.0.0 COMPATIBILITY AnyNewerVersion)

# see https://discourse.cmake.org/t/exit-with-error-if-user-attempt-to-override-cmake-install-prefix-during-installation/10020/3
# if(NOT CMAKE_INSTALL_PREFIX STREQUAL "@CMAKE_INSTALL_PREFIX@")
#     message(FATAL_ERROR "wrong CMAKE_INSTALL_PREFIX")
# endif()

configure_file(AsioConfig.cmake.in AsioConfig.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/AsioConfig.cmake DESTINATION lib/cmake/Asio)


cmake_minimum_required(VERSION 3.8)

set(PLATFORM_NAME "x86_64" CACHE STRING "Platform name defaulting to 'x86_64'")
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/boot/${PLATFORM_NAME}/toolchain.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
enable_language(C CXX ASM)

include(Kernel)

project(libsupxx)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/libsupcxx/include
  )

add_subdirectory(tests)
add_subdirectory(io)
add_subdirectory(libsupcxx/src)
add_subdirectory(libruncxx)
add_subdirectory(boot/${PLATFORM_NAME})

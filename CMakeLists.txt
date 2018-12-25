
cmake_minimum_required(VERSION 3.8)

set(TOOLCHAIN_NAME "i686-elf" CACHE STRING "Toolchain name defaulting to 'i686-elf'")
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/${TOOLCHAIN_NAME}.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
enable_language(C CXX ASM)

include(Kernel)

project(libsupxx)

add_subdirectory(boot/${ARCH_TYPE})

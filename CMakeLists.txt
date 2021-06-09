
cmake_minimum_required(VERSION 3.8)

# Includes in this project are meant for dbt, so they are relative to dbt's DEPS folder,
# where all modules, including the main project, would have their own folder.
# So every include to files in this project is prefixed by the name of the project folder.
# For example, to include io/io.hh we use #include "libsupxx/io/io.hh"
# 
# To make CMake understand this structure, we recreate the DEPS folder inside CMake's 
# ${CMAKE_BINARY_DIR} folder. This is NOT in the same place as dbt's DEPS or BUILD folder,
# so CMake and dbt are kept separate, and can be used at the same time.

# Make the CMAKE_DEPS folder (full path ${DBT_INCLUDE_ROOT}) where we recreate dbt's DEPS
set(DBT_INCLUDE_ROOT ${CMAKE_BINARY_DIR}/CMAKE_DEPS)
file(MAKE_DIRECTORY ${DBT_INCLUDE_ROOT})

# This project's dbt name (its main directory name, not necessarily CMake's PROJECT_NAME)
get_filename_component(
  DBT_PROJECT_NAME 
  ${CMAKE_SOURCE_DIR} NAME
  )

# Symlink this project's source dir in CMAKE_DEPS (${DBT_INCLUDE_ROOT})
execute_process(
  COMMAND ln -sfn ${CMAKE_SOURCE_DIR} ${DBT_INCLUDE_ROOT}/${DBT_PROJECT_NAME}
  )

# Copy or symlink any other dependencies in the same ${DBT_INCLUDE_ROOT} folder
# (there aren't any here)


set(PLATFORM_NAME "x86_64" CACHE STRING "Platform name defaulting to 'x86_64'")
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/boot/${PLATFORM_NAME}/toolchain.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
enable_language(C CXX ASM)

include(Kernel)

project(libsupxx)

# Make includes relative to the CMAKE_DEPS folder, but include stdlib headers normally
include_directories(
  ${DBT_INCLUDE_ROOT}
  ${CMAKE_SOURCE_DIR}/libsupcxx/include
  )

add_subdirectory(tests)
add_subdirectory(io)
add_subdirectory(libsupcxx/src)
add_subdirectory(libruncxx)
add_subdirectory(boot/${PLATFORM_NAME})

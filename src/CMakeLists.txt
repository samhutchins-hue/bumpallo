cmake_minimum_required(VERSION 3.16)
project(exercises)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
  ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE debug)
endif()

# ASan and UBSan
# UBSan for misalignment
# ASan for buffer overflows and use-after-free
# -fno-omit-frame-pointer, stack traces accurate and readable
# can use UBSAN_OPTIONS=halt_on_error=1 ./build/bumpallo
add_compile_options(-fsanitize=address -fsanitize=undefined -g -fno-omit-frame-pointer)
add_link_options(-fsanitize=address -fsanitize=undefined)

file(GLOB SOURCES "*.cpp")
foreach(source ${SOURCES})
  get_filename_component(name ${source} NAME_WE)
  add_executable(${name} ${source})
endforeach()

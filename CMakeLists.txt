cmake_minimum_required(VERSION 3.11)

project(heckel_diff)

set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory(src)
add_subdirectory(example)
add_subdirectory(tests)

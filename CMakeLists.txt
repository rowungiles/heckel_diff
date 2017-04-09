cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Werror -Wextra")

set(BINARY_NAME bin_heckel_diff)
set(MAIN_PROJECT_NAME heckel_diff)

add_subdirectory(src)
add_subdirectory(test)
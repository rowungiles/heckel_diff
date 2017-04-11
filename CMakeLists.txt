cmake_minimum_required(VERSION 3.7)

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
    SET(CMAKE_CXX_FLAGS "-Wall -Werror -Wextra -std=c++14 -stdlib=libc++")
ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
    SET(CMAKE_CXX_FLAGS "-Wall -Werror -Wextra -std=c++14")
ENDIF()

set(ENV{LIB_NAME} heckel_diff)
set(ENV{BIN_NAME} bin_heckel_diff)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(example)
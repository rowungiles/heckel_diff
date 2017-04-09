cmake_minimum_required(VERSION 3.7)

project(heckel_diff)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Werror -Wextra")

set(SOURCES
    main.cpp
        hd_algorithm.cpp
)

add_library(${PROJECT_NAME} ${SOURCES})
add_executable(bin_${PROJECT_NAME} ${SOURCES})

target_link_libraries(bin_${PROJECT_NAME} ${PROJECT_NAME})
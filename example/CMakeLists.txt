cmake_minimum_required(VERSION 3.7)

project(example_heckel_diff)

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/heckel_diff_example.cpp
        )

find_library(${LIBRARY} ${LIB_NAME}
        HINTS ${CMAKE_BINARY_DIR}/src)

include_directories(${CMAKE_BINARY_DIR}/src/include)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARY})
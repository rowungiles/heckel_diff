cmake_minimum_required(VERSION 3.7)

project(example_heckel_diff)

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/heckel_diff_example.cpp
        )

include_directories(${CMAKE_BINARY_DIR}/src/include)

add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties($ENV{LIB_NAME} PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/example)
add_library($ENV{LIB_NAME}_example SHARED IMPORTED GLOBAL)
target_link_libraries(${PROJECT_NAME} $ENV{LIB_NAME})
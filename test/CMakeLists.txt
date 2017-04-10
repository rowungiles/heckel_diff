cmake_minimum_required(VERSION 3.7)

project(heckel_diff_tests)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
 
# Add googletest directly to our build. This adds
# the following targets: gtest, gtest_main, gmock
# and gmock_main
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
)

find_library(${LIBRARY} ${LIB_NAME}
        HINTS ${CMAKE_BINARY_DIR}/src)

include_directories(${CMAKE_BINARY_DIR}/src/include)

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/algorithm_tests.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} gtest ${LIBRARY})

cmake_minimum_required(VERSION 3.7)

project(heckel_diff)

find_program(MEMORYCHECK_COMMAND valgrind)
if(MEMORYCHECK_COMMAND)
    message("Valgrind: ${MEMORYCHECK_COMMAND}")
    set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")

    include(CTest)

    add_custom_target(valgrind
            COMMAND ${CMAKE_CTEST_COMMAND}
            --force-new-ctest-process --test-action memcheck
            COMMAND cat "${CMAKE_BINARY_DIR}/test/tmp/MemoryChecker.*.log")

    add_test(valgrind $ENV{BIN_NAME})

else()
    message("Valgrind not found")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/heckel_diff.hpp ${CMAKE_BINARY_DIR}/src/include/heckel_diff/heckel_diff.hpp COPYONLY)

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/heckel_diff.cpp
        )

add_executable($ENV{BIN_NAME} ${SOURCES} ${INCLUDES})

set(LIB_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/heckel_diff.cpp
        )

add_library($ENV{LIB_NAME} SHARED ${LIB_SOURCES})
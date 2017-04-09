cmake_minimum_required(VERSION 3.7)

project(heckel_diff)

find_program(MEMORYCHECK_COMMAND valgrind)
if(MEMORYCHECK_COMMAND)
    message("Valgrind: ${MEMORYCHECK_COMMAND}")
    set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")

    include(CTest)

    add_custom_target(test_memcheck
            COMMAND ${CMAKE_CTEST_COMMAND}
            --force-new-ctest-process --test-action memcheck
            COMMAND cat "${CMAKE_BINARY_DIR}/test/tmp/MemoryChecker.*.log")

    add_test(valgrind ${BINARY_NAME})

else()
    message("Valgrind not found")
endif()

set(SOURCES
        main.cpp
        hd_algorithm.cpp
        )

add_library(${PROJECT_NAME} ${SOURCES})
add_executable(${BINARY_NAME} ${SOURCES})

target_link_libraries(${BINARY_NAME} ${PROJECT_NAME})
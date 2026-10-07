cmake_minimum_required(VERSION 3.16)
project(lab4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(LAB4_COVERAGE "Enable coverage instrumentation" OFF)

set(LAB4_HEADERS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(cocktail_sort STATIC src/cocktail_sort/cocktail_sort.cpp)
add_library(strand_sort STATIC src/strand_sort/strand_sort.cpp)
add_library(oriented_graph STATIC src/oriented_graph/oriented_graph.cpp)

foreach(target cocktail_sort strand_sort oriented_graph)
    target_include_directories(${target} PUBLIC ${LAB4_HEADERS_ROOT})
endforeach()

add_executable(lab4 src/main.cpp)
target_link_libraries(lab4 PRIVATE cocktail_sort strand_sort oriented_graph)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    foreach(target cocktail_sort strand_sort oriented_graph lab4)
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endforeach()
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(UNITTESTPP REQUIRED IMPORTED_TARGET UnitTest++)

add_executable(lab4_tests
    tests/test_main.cpp
    tests/test_cocktail_sort.cpp
    tests/test_strand_sort.cpp
    tests/test_graph.cpp)
target_link_libraries(lab4_tests PRIVATE cocktail_sort strand_sort oriented_graph PkgConfig::UNITTESTPP)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(lab4_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME unit_tests COMMAND lab4_tests)

if(LAB4_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0 -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

    find_program(GCOV gcov)
    find_program(GCOVR gcovr)
    find_program(LCOV lcov)

    if(GCOVR)
        add_custom_target(coverage
            COMMAND ${GCOVR} --root ${CMAKE_CURRENT_SOURCE_DIR}
                    --filter ${CMAKE_CURRENT_SOURCE_DIR}/include
                    --filter ${CMAKE_CURRENT_SOURCE_DIR}/src
                    --exclude ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
                    --exclude ${CMAKE_CURRENT_SOURCE_DIR}/src/cli
                    --print-summary
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS lab4_tests)
    elseif(LCOV)
        add_custom_target(coverage
            COMMAND ${LCOV} --capture --directory . --output-file coverage.info
                    --rc lcov_branch_coverage=1
            COMMAND genhtml coverage.info --output-directory coverage_report
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS lab4_tests)
    endif()
endif()

if(CMAKE_EXPORT_COMPILE_COMMANDS AND NOT WIN32)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
                ${CMAKE_BINARY_DIR}/compile_commands.json
                ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
        ERROR_QUIET)
endif()

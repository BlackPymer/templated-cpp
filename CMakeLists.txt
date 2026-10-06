cmake_minimum_required(VERSION 3.16)
project(lab4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cocktail_sort STATIC src/cocktail_sort/cocktail_sort.cpp)
target_include_directories(cocktail_sort PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(oriented_graph STATIC src/oriented_graph/oriented_graph.cpp)
target_include_directories(oriented_graph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(lab4 src/main.cpp)
target_link_libraries(lab4 PRIVATE cocktail_sort oriented_graph)

foreach(target cocktail_sort oriented_graph lab4)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()

if(CMAKE_EXPORT_COMPILE_COMMANDS AND NOT WIN32)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
                ${CMAKE_BINARY_DIR}/compile_commands.json
                ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
        ERROR_QUIET)
endif()

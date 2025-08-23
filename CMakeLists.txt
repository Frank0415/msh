cmake_minimum_required(VERSION 3.10)

project(my_shell C)

option(USE_NDEBUG "Disable assertions" OFF)

add_executable(my_shell
    src/main.c
    src/parse.c
    src/memalloc.c
)

target_include_directories(my_shell PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(my_shell PRIVATE -Wall -Wextra -Werror -std=gnu11 -O2)

if(USE_NDEBUG)
    target_compile_definitions(my_shell PRIVATE NDEBUG)
endif()
cmake_minimum_required(VERSION 3.10)

project(my_shell C)

target_include_directories(my_shell PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(my_shell
    src/main.c
)
cmake_minimum_required(VERSION 3.27)

project(
        Task_Scheduler
        VERSION 1.0
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(bin)
add_subdirectory(lib)

enable_testing()
add_subdirectory(tests)
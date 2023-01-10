cmake_minimum_required (VERSION 3.0)
project (asst1)

# set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_STANDARD 20)

include_directories(".")

add_subdirectory (niGraphReader)

find_program(CLANG_TIDY_EXE NAMES clang-tidy-12 REQUIRED)
set(CMAKE_CXX_CLANG_TIDY clang-tidy-12 -checks=bugprone-*,clang-analyzer-*,cppcoreguidelines-*,-cppcoreguidelines-pro-bounds-array-to-pointer-decay)

add_subdirectory (niGraph)

add_executable (main 
    main.cpp
    utils.cpp
    solutions.cpp
)

target_link_libraries(main niGraph niGraphReader)

cmake_minimum_required (VERSION 3.0)
project (asst1)

include_directories(".")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory (niGraph)
add_subdirectory (niGraphReader)

set(CMAKE_BUILD_TYPE Debug)

add_executable (main 
    main.cpp
    utils.cpp
    
    solutions.cpp
    )

target_link_libraries(main niGraph niGraphReader)
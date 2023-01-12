cmake_minimum_required(VERSION 3.13)
project(625_llvm)

set(CMAKE_BUILD_TYPE Debug)

find_package ( LLVM 12.0 REQUIRED CONFIG )
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

include(HandleLLVMOptions)
include(AddLLVM)

add_definitions ( ${LLVM_DEFINITIONS})
include_directories ( ${LLVM_INCLUDE_DIRS})

add_llvm_library(ATB_625 MODULE 
    AdderTreeBalancer.cpp
    PLUGIN_TOOL opt
)

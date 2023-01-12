cmake_minimum_required(VERSION 3.13)
project(625_scheduler)

set(CMAKE_BUILD_TYPE Debug)

# set ( COAST_BASE_DIR ..)
# if(NOT EXISTS ${COAST_BASE_DIR})
# 	message ( FATAL_ERROR " COAST_BASE_DIR (${COAST_BASE_DIR}) is invalid ")
# endif ()

# set(LLVM_DIR ../build/cmake/modules/CMakeFiles)

find_package ( LLVM 12.0 REQUIRED CONFIG )
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")


list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

include(HandleLLVMOptions)
include(AddLLVM)


# list (APPEND LLVM_INCLUDE_DIRS "${COAST_BASE_DIR}/llvm-project/llvm/include")

add_definitions ( ${LLVM_DEFINITIONS})
include_directories ( ${LLVM_INCLUDE_DIRS})

# add_compile_options("-D_REENTRANT")
# add_compile_options("-fPIC")

add_llvm_library(scheduler_625 MODULE 
    Scheduler625.cpp 
    Scheduler625soln.cpp
    FunctionalUnit.cpp 
    SchedHelper.cpp
    FunctionHLS.cpp
    PLUGIN_TOOL opt
)
target_link_libraries(scheduler_625 PRIVATE lpsolve55_pic)


# include_directories ( ${LLVM_INCLUDE_DIRS})

# add_subdirectory (debugStatements)
# add_subdirectory (exitMarker)
# add_subdirectory (errorBlocks)
# add_subdirectory (CFCSS)
# add_subdirectory (dataflowProtection)
# add_subdirectory (DWC)
# add_subdirectory (TMR)
# add_subdirectory (smallProfile)
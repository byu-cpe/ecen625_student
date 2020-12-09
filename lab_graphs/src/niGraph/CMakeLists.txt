file(GLOB SOURCES *.cpp *.h)

add_library(niGraph ${SOURCES})

#include_directories(".")
target_include_directories(niGraph PUBLIC ".")

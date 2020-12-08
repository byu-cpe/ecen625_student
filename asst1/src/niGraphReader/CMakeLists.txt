file(GLOB SOURCES *.cpp *.h)

add_library(niGraphReader ${SOURCES})

target_link_libraries(niGraphReader niGraph)
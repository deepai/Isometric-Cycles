cmake_minimum_required(VERSION 3.0)

# Pass options to NVCC

add_compile_options(-O2 -std=c++11 -fopenmp -w)
add_definitions(-DMULTI_THREAD -DNDEBUG -DVERBOSE ) #-DPRINT_CYCLES) # -DPRINT)

include_directories(
    include
    )

FILE(GLOB HOST_SOURCES  "include/*.cpp" "include/*.h" "include/*.hpp")

add_library(hostlib OBJECT ${HOST_SOURCES})

add_executable(
	Test_Isometric_cycles
	src/Test_Isometric_cycles.cpp
	$<TARGET_OBJECTS:hostlib>
)

add_executable(
	Planar_Verify_Generate
	src/Creation_graph.cpp
	$<TARGET_OBJECTS:hostlib>
)

target_link_libraries(
    Test_Isometric_cycles
    -lgomp
    )

target_link_libraries(
    Planar_Verify_Generate
    -lgomp
    )
set_target_properties( Test_Isometric_cycles
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set_target_properties( Planar_Verify_Generate
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

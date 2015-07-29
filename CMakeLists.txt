
# === CMake lists for the GeometronLib - (26/07/2015) ===

cmake_minimum_required(VERSION 2.8)
project(GeometronLib)


# === Build path ===

set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR} CACHE PATH "Build directory" FORCE)
set(PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(PROJECT_TEST_DIR "${PROJECT_SOURCE_DIR}/test")


# === Global files ===

file(GLOB_RECURSE HeadersAll "${PROJECT_SOURCE_DIR}/include/*.*")
file(GLOB_RECURSE SourcesAll "${PROJECT_SOURCE_DIR}/sources/*.*")

set(FilesAll ${HeadersAll} ${SourcesAll})


# === Include directories ===

include_directories("${PROJECT_INCLUDE_DIR}")

find_path(GaussianLib_INCLUDE_DIR NAMES Gauss/Gauss.h)
include_directories(${GaussianLib_INCLUDE_DIR})


# === Binaries ===

add_library(geomlib STATIC ${FilesAll})
set_target_properties(geomlib PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")

add_executable(test1 "${PROJECT_TEST_DIR}/test1.cpp")
set_target_properties(test1 PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")

target_link_libraries(test1 geomlib)


